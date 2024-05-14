#include "PCH.hpp"

#include "../include/Rendering/renderDefs.h"
#include "fx/gltf.h"
#include "string"
// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "resourceloader.hpp"
using namespace Resources;

//std::pair for some odd reason silently fails on emplace back, making a simple struct seemed to fix it.
struct AttributeTypePair {
  VertexAttributeType first;
  unsigned second;
};

template<typename T>
concept Copyable = std::is_trivially_copyable<T>::value;

template<typename T>
T ReadByteData(unsigned char const *begin, unsigned char const *const end) requires Copyable<T> {
  T t;
  std::memcpy(&t, begin, sizeof(t));
  begin += sizeof(t);
  return t;
}

//Calling with a beginning has fewer guarantees than calling a beginning and end.
template<typename T>
T ReadByteData(unsigned char const *begin) requires Copyable<T> {
  T t;
  std::memcpy(&t, begin, sizeof(t));
  return t;
}

std::vector<AttributeTypePair> ParseAttribute(const fx::gltf::Attributes &attributes);
void AssignIndices(Mesh &model,
				   const fx::gltf::BufferView &bufferView,
				   const fx::gltf::Accessor &accessor,
				   const fx::gltf::Buffer &buffer);
void AssignVectorValues(std::vector<glm::vec3> &vector,
						const fx::gltf::BufferView &bufferView,
						const fx::gltf::Accessor &accessor,
						const fx::gltf::Buffer &buffer);
void AssignVectorValues(std::vector<glm::vec2> &vector,
						const fx::gltf::BufferView &bufferView,
						const fx::gltf::Accessor &accessor,
						const fx::gltf::Buffer &buffer);
void AssignVectorValues(std::vector<glm::vec4> &vector,
						const fx::gltf::BufferView &bufferView,
						const fx::gltf::Accessor &accessor,
						const fx::gltf::Buffer &buffer);

Mesh ParseMesh(const fx::gltf::Mesh &mesh, const fx::gltf::Document &doc);
std::shared_ptr<Model> GenerateGLTFModelRef(const std::string &filePath);
Texture GenerateTexReference(const std::string &filepath);
ResourceLoader *ResourceLoader::instance;

ResourceLoader *ResourceLoader::CreateResourceLoader() {
  if (!instance) {
	instance = new ResourceLoader;
  }

  return instance;
}

ResourceLoader *ResourceLoader::GetHandle() {
  return instance;
}
bool ResourceLoader::GenerateResource(const toml::parse_result &toml) {
  u64 id = std::hash<std::string>{}(toml["Entity"]["nickname"].value_or(""));
  const std::string filePath = std::format("{}{}", debugRoot, toml["Resource"]["model_path"].value_or(""));
  modelReferences.insert({id, GenerateGLTFModelRef(filePath)});
  modelReferences[id]->archetypeId = id;

  return true;
}
std::weak_ptr<Resources::Model> ResourceLoader::GrabModelRef(u64 id) {
  if (modelReferences.find(id) != modelReferences.end()) {
	return modelReferences[id];
  }

  GenerateResource(ConfigLoader::GetHandle()->LoadConfigFile(id));
  return modelReferences[id];
}

std::shared_ptr<Model> GenerateGLTFModelRef(const std::string &filePath) {
  const fx::gltf::Document &doc = fx::gltf::LoadFromText(filePath);

  const auto modelDirectory = fx::gltf::detail::GetDocumentRootPath(filePath);

  std::shared_ptr<Model> modelRef = std::make_shared<Model>();
  for (const auto &mesh : doc.meshes) {
	modelRef->meshes.emplace_back(ParseMesh(mesh, doc));
  }

  for (const auto &img : doc.images) {
	modelRef->textures.emplace_back(GenerateTexReference(modelDirectory.string() + "/" + img.uri));
  }

  for (const auto &mat : doc.materials) {
	Material matRef{};
	matRef.normalIndex = mat.normalTexture.index;
	matRef.albedoIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
	matRef.metallicRoughnessIndex = mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
	matRef.aoIndex = mat.occlusionTexture.index;
	matRef.emissiveIndex = mat.emissiveTexture.index;
	matRef.emissiveFactor = {mat.emissiveFactor[0], mat.emissiveFactor[1], mat.emissiveFactor[2]};
	modelRef->materials.emplace_back(matRef);
  }

  return modelRef;
}

Mesh ParseMesh(const fx::gltf::Mesh &mesh, const fx::gltf::Document &doc) {
  Mesh meshRef;
  //Find out what primitive type the mesh describes
  for (const auto &i : mesh.primitives) {
	meshRef.mode = static_cast<Mode>(i.mode);
  }
  //Grab the primitive attributes to know which buffers mean what
  //Grab which bufferView and accessor describes what vertex attributes and which buffer describes the index.
  const auto ibv = mesh.primitives[0].indices;
  const auto parsedAttributes = ParseAttribute(mesh.primitives[0].attributes);
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec4> tangents;
  std::vector<glm::vec2> texCoords;
  std::vector<glm::vec2> colors;


  //Assign them to the Resource OpenGLModel
  AssignIndices(meshRef, doc.bufferViews[ibv], doc.accessors[ibv], doc.buffers[doc.bufferViews[ibv].buffer]);
  for (const auto &i : parsedAttributes) {
#pragma warning (disable : 0304)

	const auto &accessor = doc.accessors[i.second];
	const auto &bufferView = doc.bufferViews[accessor.bufferView];

	const auto &buffer = doc.buffers[doc.bufferViews[accessor.bufferView].buffer];

	switch (i.first) {
	  case position: {
		AssignVectorValues(positions, bufferView, accessor, buffer);
		break;
	  }
	  case normal: {
		AssignVectorValues(normals, bufferView, accessor, buffer);
		break;
	  }
	  case tangent: {
		AssignVectorValues(tangents, bufferView, accessor, buffer);
		break;
	  }
	  case texcoord_n: {
		AssignVectorValues(texCoords, bufferView, accessor, buffer);
		break;
	  }
	  case color_n: {
		AssignVectorValues(colors, bufferView, accessor, buffer);
		break;
	  }
	}
  }
  for (const auto i : positions) {
	Vertex vertex{};
	vertex.position = i;
	meshRef.vertexes.emplace_back(vertex);
  }

  for (int i = 0; i < normals.size(); i++) {
	meshRef.vertexes[i].normal = normals[i];
  }

  for (int i = 0; i < tangents.size(); i++) {
	meshRef.vertexes[i].tangent = tangents[i];
  }

  for (int i = 0; i < texCoords.size(); i++) {
	meshRef.vertexes[i].texcoordinates = texCoords[i];
  }

  for (int i = 0; i < colors.size(); i++) {
	meshRef.vertexes[i].color = colors[i];
  }

  for (int i = 0; i < tangents.size(); i++) {
	meshRef.vertexes[i].bitangent =
		glm::cross(normals[i], {tangents[i].x, tangents[i].y, tangents[i].z}) * tangents[i].w;
  }

  meshRef.archetypeId = std::hash<std::string>{}(mesh.name);
  return meshRef;
}

std::vector<AttributeTypePair> ParseAttribute(const fx::gltf::Attributes &attributes) {
  std::vector<AttributeTypePair> attributeAccessorPairs;
  AttributeTypePair vertexAttribute = {};
  for (const auto &att : attributes) {
	if (att.first == "POSITION") {
	  vertexAttribute.first = position;
	  vertexAttribute.second = att.second;
	} else if (att.first == "NORMAL") {
	  vertexAttribute.first = normal;
	  vertexAttribute.second = att.second;
	} else if (att.first == "TANGENT") {
	  vertexAttribute.first = tangent;
	  vertexAttribute.second = att.second;
	} else if (att.first == "TEXCOORD_0") {
	  vertexAttribute.first = texcoord_n;
	  vertexAttribute.second = att.second;
	} else if (att.first == "COLOR_n") {
	  vertexAttribute.first = color_n;
	  vertexAttribute.second = att.second;
	}
	attributeAccessorPairs.emplace_back(vertexAttribute);
  }
  return attributeAccessorPairs;
}

void AssignIndices(Mesh &model,
				   const fx::gltf::BufferView &bufferView,
				   const fx::gltf::Accessor &accessor,
				   const fx::gltf::Buffer &buffer) {
  const auto &data = buffer.data;

  if (accessor.componentType == fx::gltf::Accessor::ComponentType::UnsignedShort) {
	for (auto i = bufferView.byteOffset; i < bufferView.byteLength + bufferView.byteOffset; i += sizeof(u16)) {
	  const auto offset = &data[i];
	  u32 index = ReadByteData<u16>(offset);
	  model.indices.emplace_back(index);
	}
  } else if (accessor.componentType == fx::gltf::Accessor::ComponentType::UnsignedInt) {
	for (auto i = bufferView.byteOffset; i < bufferView.byteLength + bufferView.byteOffset; i += sizeof(u32)) {
	  const auto offset = &data[i];
	  u32 index = ReadByteData<u32>(offset);
	  model.indices.emplace_back(index);
	}
  }

}

void AssignVectorValues(std::vector<glm::vec3> &vector,
						const fx::gltf::BufferView &bufferView,
						const fx::gltf::Accessor &accessor,
						const fx::gltf::Buffer &buffer) {
  const auto &data = buffer.data;

  for (auto i = bufferView.byteOffset; i < bufferView.byteLength + bufferView.byteOffset; i += sizeof(glm::vec3)) {
	auto offset = &data[i];
	auto vec = ReadByteData<glm::vec3>(offset);
	vector.emplace_back(vec);
  }
}

void AssignVectorValues(std::vector<glm::vec2> &vector,
						const fx::gltf::BufferView &bufferView,
						const fx::gltf::Accessor &accessor,
						const fx::gltf::Buffer &buffer) {
  const auto &data = buffer.data;

  for (auto i = bufferView.byteOffset; i < bufferView.byteLength + bufferView.byteOffset; i += sizeof(glm::vec2)) {
	const auto offset = &data[i];
	const auto vec = ReadByteData<glm::vec2>(offset);
	vector.emplace_back(vec);
  }
}
void AssignVectorValues(std::vector<glm::vec4> &vector,
						const fx::gltf::BufferView &bufferView,
						const fx::gltf::Accessor &accessor,
						const fx::gltf::Buffer &buffer) {
  const auto &data = buffer.data;

  for (auto i = bufferView.byteOffset; i < bufferView.byteLength + bufferView.byteOffset; i += sizeof(glm::vec4)) {
	const auto offset = &data[i];
	const auto vec = ReadByteData<glm::vec4>(offset);
	vector.emplace_back(vec);
  }
}

Texture GenerateTexReference(const std::string &filepath) {
  Texture textureRef = {};
  textureRef.textureData =
	  stbi_load(filepath.c_str(), &textureRef.width, &textureRef.height, &textureRef.componentNumber, 0);
  textureRef.archetypeId = std::hash<std::string>{}(filepath);
  return textureRef;
}

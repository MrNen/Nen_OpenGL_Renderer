#pragma once

class GameState {
  u64 timeStamp;

 public:
  void GenerateNewGameState(const GameState *previousUpdate, u64 time);

  u64 GetTimeStamp();

};

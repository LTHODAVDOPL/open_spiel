// Copyright 2019 DeepMind Technologies Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef THIRD_PARTY_OPEN_SPIEL_GAMES_CATCH_H_
#define THIRD_PARTY_OPEN_SPIEL_GAMES_CATCH_H_

#include <memory>
#include <string>
#include <vector>

#include "open_spiel/spiel.h"

// Catch is a single player game, often used for unit testing RL algorithms.
//
// The player must move a paddle to intercept a falling ball. The initial
// column of the ball is decided by chance. Each turn, the ball moves downwards
// while remaining in the initial column.
//
// Please note: In each turn, all actions (left, stay, right) are legal. This
// is different to the Python implementation of the game.
//
// References:
// a) Recurrent models of visual attention, 2014, Minh et al.
//    (Advances in Neural Information Processing Systems 27, pages 2204–2212.)
// b) Behaviour Suite for Reinforcement Learning, 2019, Osband et al.
//    (https://arxiv.org/abs/1908.03568)
//
// Parameters:
//  "rows"       int    rows of the board        (default = 10)
//  "columns"    int    columns of the board     (default = 5)

namespace open_spiel {
namespace catch_ {

// Constants.
constexpr int kNumPlayers = 1;
constexpr int kNumActions = 3;
constexpr int kDefaultRows = 10;
constexpr int kDefaultColumns = 5;

// State of a cell.
enum class CellState {
  kEmpty,
  kBall,
  kPaddle,
};

class CatchGame;

// State of an in-play game.
class CatchState : public State {
 public:
  CatchState(const CatchGame& parent_game);
  CatchState(const CatchState&) = default;

  int CurrentPlayer() const override;
  std::string ActionToString(int player, Action action_id) const override;
  std::string ToString() const override;
  bool IsTerminal() const override;
  std::vector<double> Returns() const override;
  std::string InformationState(int player) const override;
  std::string Observation(int player) const override;
  void ObservationAsNormalizedVector(
      int player, std::vector<double>* values) const override;
  void InformationStateAsNormalizedVector(
      int player, std::vector<double>* values) const override;
  std::unique_ptr<State> Clone() const override;
  void UndoAction(int player, Action move) override;
  std::vector<Action> LegalActions() const override;
  ActionsAndProbs ChanceOutcomes() const override;
  CellState BoardAt(int row, int column) const;

 protected:
  void DoApplyAction(Action move) override;

 private:
  const CatchGame& game_;

  bool initialized_ = false;
  int ball_row_ = -1;
  int ball_col_ = -1;
  int paddle_col_ = -1;
};

// Game object.
class CatchGame : public Game {
 public:
  explicit CatchGame(const GameParameters& params);
  std::unique_ptr<State> NewInitialState() const override {
    return std::unique_ptr<State>(new CatchState(*this));
  }
  std::unique_ptr<Game> Clone() const override {
    return std::unique_ptr<Game>(new CatchGame(*this));
  }
  std::vector<int> ObservationNormalizedVectorShape() const override {
    return {num_rows_, num_columns_};
  }
  std::vector<int> InformationStateNormalizedVectorShape() const override {
    return {num_columns_ + kNumActions * num_rows_};
  }

  int NumDistinctActions() const override { return kNumActions; }
  int MaxChanceOutcomes() const override { return num_columns_; }
  int NumPlayers() const override { return kNumPlayers; }
  double MaxUtility() const override { return 1; }
  double MinUtility() const override { return -1; }
  int MaxGameLength() const override { return num_rows_; }
  int NumRows() const { return num_rows_; }
  int NumColumns() const { return num_columns_; }

 private:
  const int num_rows_;
  const int num_columns_;
};

}  // namespace catch_
}  // namespace open_spiel

#endif  // THIRD_PARTY_OPEN_SPIEL_GAMES_CATCH_H_

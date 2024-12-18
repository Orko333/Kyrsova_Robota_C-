#ifndef CHOMP_AI_HPP
#define CHOMP_AI_HPP

#include <vector>
#include <cstdint>
#include <random>
#include <ctime>
#include <algorithm>
#include <limits>

extern "C" {
    typedef struct {
        int32_t row;
        int32_t col;
    } Move;

    // Стратегія мінімакс для гри Chomp
    class ChompAI {
    private:
        // Визначає, чи є хід безпечним (не включає отруйний блок)
        bool is_safe_move(int32_t row, int32_t col, int32_t poison_row, int32_t poison_col) {
            return !(row == poison_row && col == poison_col);
        }

        // Перевіряє, чи є дошка в термінальному стані (немає доступних ходів)
        bool is_terminal_state(std::vector<std::vector<int32_t>>& board) {
            for (const auto& row : board) {
                if (std::find(row.begin(), row.end(), 1) != row.end()) {
                    return false;
                }
            }
            return true;
        }

        // Евристична оцінка стану дошки
        int32_t evaluate_board(std::vector<std::vector<int32_t>>& board) {
            int32_t remaining_blocks = 0;
            for (const auto& row : board) {
                remaining_blocks += std::count(row.begin(), row.end(), 1);
            }
            return remaining_blocks;
        }

        // Імітує хід на дошці
        void make_move(std::vector<std::vector<int32_t>>& board, Move move) {
            for (int32_t i = 0; i <= move.row; ++i) {
                for (int32_t j = 0; j <= move.col; ++j) {
                    board[i][j] = 0;
                }
            }
        }

        // Алгоритм мінімакс з альфа-бета відсіканням
        int32_t minimax(std::vector<std::vector<int32_t>>& board, int32_t depth, bool is_maximizing,
                        int32_t poison_row, int32_t poison_col, int32_t alpha, int32_t beta) {
            // Термінальний стан або досягнуто обмеження глибини
            if (depth == 0 || is_terminal_state(board)) {
                return evaluate_board(board);
            }

            if (is_maximizing) {
                int32_t max_eval = std::numeric_limits<int32_t>::min();
                for (int32_t row = 0; row < board.size(); ++row) {
                    for (int32_t col = 0; col < board[0].size(); ++col) {
                        if (board[row][col] == 1 && is_safe_move(row, col, poison_row, poison_col)) {
                            auto board_copy = board;
                            make_move(board_copy, {row, col});
                            int32_t eval = minimax(board_copy, depth - 1, false, poison_row, poison_col, alpha, beta);
                            max_eval = std::max(max_eval, eval);
                            alpha = std::max(alpha, eval);
                            if (beta <= alpha) {
                                break; // Відсікання бета
                            }
                        }
                    }
                }
                return max_eval;
            } else {
                int32_t min_eval = std::numeric_limits<int32_t>::max();
                for (int32_t row = 0; row < board.size(); ++row) {
                    for (int32_t col = 0; col < board[0].size(); ++col) {
                        if (board[row][col] == 1 && is_safe_move(row, col, poison_row, poison_col)) {
                            auto board_copy = board;
                            make_move(board_copy, {row, col});
                            int32_t eval = minimax(board_copy, depth - 1, true, poison_row, poison_col, alpha, beta);
                            min_eval = std::min(min_eval, eval);
                            beta = std::min(beta, eval);
                            if (beta <= alpha) {
                                break; // Відсікання альфа
                            }
                        }
                    }
                }
                return min_eval;
            }
        }

    public:
        // Основна функція для вибору найкращого ходу
        Move choose_best_move(std::vector<std::vector<int32_t>>& board, int32_t poison_row, int32_t poison_col) {
            Move best_move = {poison_row, poison_col};
            int32_t best_eval = std::numeric_limits<int32_t>::min();

            // Спробуйте різні глибини пошуку залежно від складності дошки
            int32_t depth = std::min(5, static_cast<int32_t>(board.size() * board[0].size()));

            // Випадковість для запобігання передбачуваності
            std::mt19937 rng(std::time(nullptr));
            std::uniform_int_distribution<int32_t> dist(0, 100);

            for (int32_t row = 0; row < board.size(); ++row) {
                for (int32_t col = 0; col < board[0].size(); ++col) {
                    if (board[row][col] == 1 && is_safe_move(row, col, poison_row, poison_col)) {
                        auto board_copy = board;
                        make_move(board_copy, {row, col});

                        // Оцінка мінімакс з деякою випадковістю
                        int32_t eval = minimax(board_copy, depth, false, poison_row, poison_col,
                                               std::numeric_limits<int32_t>::min(),
                                               std::numeric_limits<int32_t>::max());

                        // Додайте невеликий випадковий фактор, щоб уникнути завжди вибору одного і того ж ходу
                        eval += dist(rng) / 10;

                        if (eval > best_eval) {
                            best_eval = eval;
                            best_move = {row, col};
                        }
                    }
                }
            }

            return best_move;
        }
    };

    __attribute__((visibility("default")))
    Move choose_chomp_move(int32_t* board, int32_t rows, int32_t cols, int32_t poison_row, int32_t poison_col) {
        // Перетворення плоскої дошки на 2D вектор
        std::vector<std::vector<int32_t>> game_board(rows, std::vector<int32_t>(cols));
        for (int32_t i = 0; i < rows; ++i) {
            for (int32_t j = 0; j < cols; ++j) {
                game_board[i][j] = board[i * cols + j];
            }
        }

        // Створення екземпляра AI та вибір ходу
        ChompAI ai;
        return ai.choose_best_move(game_board, poison_row, poison_col);
    }
}

#endif // CHOMP_AI_HPP
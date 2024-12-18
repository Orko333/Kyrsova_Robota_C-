#ifndef CHOMP_AI_HPP
#define CHOMP_AI_HPP

#include <vector>
#include <cstdint>
#include <random>
#include <ctime>

extern "C" {
    typedef struct {
        int32_t row;
        int32_t col;
    } Move;

    bool is_safe_move(int32_t row, int32_t col, int32_t poison_row, int32_t poison_col) {
        // Хід вважається безпечним, якщо не включає отруєний блок
        return !(row == poison_row && col == poison_col);
    }

    bool has_safe_blocks(int32_t* board, int32_t rows, int32_t cols, int32_t poison_row, int32_t poison_col) {
        // Перевіряємо, чи є хоча б один доступний блок, який не є отруєним
        for (int32_t row = 0; row < rows; ++row) {
            for (int32_t col = 0; col < cols; ++col) {
                if (board[row * cols + col] == 1 && is_safe_move(row, col, poison_row, poison_col)) {
                    return true;
                }
            }
        }
        return false;
    }

    __attribute__((visibility("default")))
    Move choose_chomp_move(int32_t* board, int32_t rows, int32_t cols, int32_t poison_row, int32_t poison_col) {
        std::vector<std::vector<int32_t>> game_board(rows, std::vector<int32_t>(cols));
        for (int32_t i = 0; i < rows; ++i) {
            for (int32_t j = 0; j < cols; ++j) {
                game_board[i][j] = board[i * cols + j];
            }
        }

        // Перевіряємо, чи є доступні не отруєні блоки
        if (!has_safe_blocks(board, rows, cols, poison_row, poison_col)) {
            return {poison_row, poison_col}; // Якщо інших ходів немає, вибираємо отруєний блок
        }

        std::vector<Move> valid_moves;
        for (int32_t row = 0; row < rows; ++row) {
            for (int32_t col = 0; col < cols; ++col) {
                if (game_board[row][col] == 1 && is_safe_move(row, col, poison_row, poison_col)) {
                    valid_moves.push_back({row, col});
                }
            }
        }

        // Якщо немає валідних ходів, повертаємо початковий хід (небажаний сценарій)
        if (valid_moves.empty()) {
            return {poison_row, poison_col};
        }

        std::mt19937 rng(std::time(nullptr));
        Move best_move = valid_moves[0];
        size_t max_blocks_removed = 0;

        for (const auto& move : valid_moves) {
            size_t blocks_removed = 0;
            for (int32_t i = 0; i <= move.row; ++i) {
                for (int32_t j = 0; j <= move.col; ++j) {
                    if (game_board[i][j] == 1) {
                        ++blocks_removed;
                    }
                }
            }

            // Вибираємо хід, який видаляє найбільшу кількість блоків
            if (blocks_removed > max_blocks_removed ||
                (blocks_removed == max_blocks_removed && rng() % 2 == 0)) {
                max_blocks_removed = blocks_removed;
                best_move = move;
            }
        }

        return best_move;
    }
}

#endif // CHOMP_AI_HPP

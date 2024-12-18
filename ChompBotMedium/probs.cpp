#include <chrono>
#include <iostream>
#include "chomp_ai.cpp"

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    // Параметри гри
    const int32_t rows = 5;  // Кількість рядків
    const int32_t cols = 5;  // Кількість стовпців
    const int32_t poison_row = 0; // Рядок отруйного блоку
    const int32_t poison_col = 0; // Стовпець отруйного блоку

    // Ігрова дошка (1 - блок доступний, 0 - блок недоступний)
    int32_t board[rows * cols] = {};
    for (int i = 0; i < rows * cols; ++i) {
        board[i] = 1;
    }


    // Виклик функції для обрання найкращого ходу
    Move best_move = choose_chomp_move(board, rows, cols, poison_row, poison_col);

    // Вивід результату
    std::cout << "Riad = " << best_move.row
              << ", Stovp = " << best_move.col << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Time: " << duration.count() << " seconds" << std::endl;
    return 0;
}

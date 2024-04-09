#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <algorithm>

#define NUM_COLORS 4
#define NUM_SHAPES 4

using namespace std;

enum class PColor { BLUE, YELLOW, RED, GREEN };
enum class PShape { SQUARE, DIAMOND, CIRCLE, TRIANGLE };

class Piece {
public:
    PColor color;
    PShape shape;
    Piece *next = nullptr;

    Piece(PColor c, PShape s) : color(c), shape(s) {}
};

class Game {
    Piece *head = nullptr;
    Piece *tail = nullptr;
    int score = 0;
    int size = 0;

    // Function to insert a piece at the end (tail) of the board
    void insertAtTail(Piece* piece) {
        if (!head) {
            head = tail = piece;
            piece->next = head; // Make it circular
        } else {
            tail->next = piece;
            tail = piece;
            tail->next = head;
        }
        size++;
    }

public:
    Game() = default;
    ~Game() {
        while (size--) {
            Piece *temp = head;
            head = head->next;
            delete temp;
        }
    }

    void insertPiece(PColor color, PShape shape) {
        Piece *newPiece = new Piece(color, shape);
        insertAtTail(newPiece);
        checkMatches(); // Automatically check for matches after each insertion
    }

    // Simplified checkMatches logic
    void checkMatches() {
        if (size < 3) return; // Early exit if not enough pieces to form a match

        bool foundMatch = false;
        do {
            Piece *current = head;
            int count = 1;
            foundMatch = false;

            while (count <= size) { // Use a count to avoid infinite loop in circular list
                Piece *start = current;
                Piece *next = current->next;

                // Count matching consecutive pieces
                while (next != head && (next->color == start->color || next->shape == start->shape)) {
                    next = next->next;
                    count++;
                }

                // If a match is found, remove the matching pieces
                if (count >= 3) {
                    foundMatch = true;
                    score += 10 * count; // Update score

                    // Remove matching pieces
                    while (count--) {
                        if (start == head) head = head->next;
                        if (start == tail) tail = tail->next;
                        Piece *temp = start->next;
                        delete start;
                        start = temp;
                        size--;
                    }

                    break; // Start checking for matches from the head again
                }

                current = current->next;
                count = 1;
            }
        } while (foundMatch); // Continue until no more matches are found
    }

    void displayBoard() const {
        cout << "Board (size " << size << "):" << endl;
        Piece *current = head;
        for (int i = 0; i < size; i++) {
            cout << "Piece " << i << ": Color " << static_cast<int>(current->color)
                 << ", Shape " << static_cast<int>(current->shape) << endl;
            current = current->next;
        }
    }

    // Additional methods as before (e.g., saveGame, loadGame, saveScore, checkScore)...
};

int main() {
    Game game;
    // Example usage
    game.insertPiece(PColor::GREEN, PShape::SQUARE);
    game.insertPiece(PColor::GREEN, PShape::DIAMOND);
    game.insertPiece(PColor::GREEN, PShape::SQUARE);
    game.insertPiece(PColor::RED, PShape::SQUARE);
    
    game.displayBoard();

    return 0;
}

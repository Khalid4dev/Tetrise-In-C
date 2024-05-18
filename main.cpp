#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

#define NUM_COLORS 4
#define NUM_SHAPES 4

using namespace std;

enum class PColor { BLUE, YELLOW, RED, GREEN };
enum class PShape { SQUARE, DIAMOND, CIRCLE, TRIANGLE };

class Piece {
public:
    PColor color;
    PShape shape;
    Piece* next = nullptr;
    Piece* prev = nullptr;

    Piece(PColor c, PShape s) : color(c), shape(s) {}
};

class Game {
    Piece* head = nullptr;
    Piece* tail = nullptr;
    int score = 0;
    int size = 0;

    // Function to insert a piece at the end (tail) of the board
    void insertAtTail(Piece* piece) {
        if (!head) {
            head = tail = piece;
            piece->next = head; // Make it circular
            piece->prev = tail; // Doubly linked
        } else {
            tail->next = piece;
            piece->prev = tail;
            tail = piece;
            tail->next = head;
            head->prev = tail;
        }
        size++;
    }

    // Function to insert a piece at the start (head) of the board
    void insertAtHead(Piece* piece) {
        if (!head) {
            head = tail = piece;
            piece->next = head; // Make it circular
            piece->prev = tail; // Doubly linked
        } else {
            piece->next = head;
            piece->prev = tail;
            head->prev = piece;
            tail->next = piece;
            head = piece;
        }
        size++;
    }

public:
    Game() = default;
    ~Game() {
        while (size--) {
            Piece* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void insertPiece(PColor color, PShape shape, bool atTail = true) {
        Piece* newPiece = new Piece(color, shape);
        if (atTail) {
            insertAtTail(newPiece);
        } else {
            insertAtHead(newPiece);
        }
        checkMatches(); // Automatically check for matches after each insertion
    }

    // Simplified checkMatches logic
    void checkMatches() {
        if (size < 3) return; // Early exit if not enough pieces to form a match

        bool foundMatch = false;
        do {
            Piece* current = head;
            int count = 1;
            foundMatch = false;

            while (count <= size) { // Use a count to avoid infinite loop in circular list
                Piece* start = current;
                Piece* next = current->next;

                // Count matching consecutive pieces
                while ((next != head) && (next->color == start->color || next->shape == start->shape)) {
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
                        if (start == tail) tail = tail->prev;
                        Piece* temp = start->next;
                        start->prev->next = start->next;
                        start->next->prev = start->prev;
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

    void shiftPiecesByColor(PColor color) {
        if (size < 2) return;
        Piece* current = head;
        do {
            if (current->color == color) {
                Piece* nextColor = current->next;
                while (nextColor != head && nextColor->color != color) {
                    nextColor = nextColor->next;
                }
                if (nextColor->color == color) {
                    swap(current->color, nextColor->color);
                }
            }
            current = current->next;
        } while (current != head);
    }

    void shiftPiecesByShape(PShape shape) {
        if (size < 2) return;
        Piece* current = head;
        do {
            if (current->shape == shape) {
                Piece* nextShape = current->next;
                while (nextShape != head && nextShape->shape != shape) {
                    nextShape = nextShape->next;
                }
                if (nextShape->shape == shape) {
                    swap(current->shape, nextShape->shape);
                }
            }
            current = current->next;
        } while (current != head);
    }

    void saveGame(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) return;

        file << size << " " << score << endl;
        Piece* current = head;
        for (int i = 0; i < size; i++) {
            file << static_cast<int>(current->color) << " " << static_cast<int>(current->shape) << endl;
            current = current->next;
        }

        file.close();
    }

    void loadGame(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return;

        int newSize, newScore;
        file >> newSize >> newScore;

        for (int i = 0; i < size; i++) {
            Piece* temp = head;
            head = head->next;
            delete temp;
        }
        head = tail = nullptr;
        size = 0;
        score = newScore;

        for (int i = 0; i < newSize; i++) {
            int color, shape;
            file >> color >> shape;
            insertPiece(static_cast<PColor>(color), static_cast<PShape>(shape));
        }

        file.close();
    }

    void saveHighScores(const string& filename, const vector<pair<string, int>>& highScores) {
        ofstream file(filename);
        if (!file.is_open()) return;

        for (const auto& entry : highScores) {
            file << entry.first << " " << entry.second << endl;
        }

        file.close();
    }

    vector<pair<string, int>> loadHighScores(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return {};

        vector<pair<string, int>> highScores;
        string name;
        int score;
        while (file >> name >> score) {
            highScores.push_back({ name, score });
        }

        file.close();
        return highScores;
    }

    void displayBoard() const {
        cout << "Board (size " << size << "):" << endl;
        Piece* current = head;
        for (int i = 0; i < size; i++) {
            cout << "Piece " << i << ": Color " << static_cast<int>(current->color)
                 << ", Shape " << static_cast<int>(current->shape) << endl;
            current = current->next;
        }
    }
};

int main() {
    Game game;
    // Example usage
    game.insertPiece(PColor::GREEN, PShape::SQUARE, true); // Insert at tail
    game.insertPiece(PColor::GREEN, PShape::DIAMOND, false); // Insert at head
    game.insertPiece(PColor::GREEN, PShape::SQUARE, true); // Insert at tail
    game.insertPiece(PColor::RED, PShape::SQUARE, false); // Insert at head
    
    game.displayBoard();

    game.shiftPiecesByColor(PColor::GREEN);
    game.displayBoard();

    game.saveGame("game_save.txt");
    game.loadGame("game_save.txt");
    game.displayBoard();

    vector<pair<string, int>> highScores = { {"Alice", 100}, {"Bob", 200} };
    game.saveHighScores("high_scores.txt", highScores);
    highScores = game.loadHighScores("high_scores.txt");

    for (const auto& entry : highScores) {
        cout << "Name: " << entry.first << ", Score: " << entry.second << endl;
    }

    return 0;
}

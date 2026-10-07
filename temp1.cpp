
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <iomanip>

using namespace std;

// ==================== HUFFMAN NODE ====================

struct Node {
    char ch;
    int frequency;

    Node* left;
    Node* right;

    Node(char c, int f) {
        ch = c;
        frequency = f;
        left = nullptr;
        right = nullptr;
    }

    // ask
    Node(Node* l, Node* r) {
        ch = '\0';
        frequency = l->frequency + r->frequency;

        left = l;
        right = r;
    }
};


// ==================== PRIORITY QUEUE ====================

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->frequency > b->frequency;
    }
};


// ==================== GLOBAL VARIABLES ====================

string inputText;
string encodedText;
string decodedText;

unordered_map<char, int> frequencyTable;
unordered_map<char, string> huffmanCodes;

Node* root = nullptr;


// ==================== FUNCTION PROTOTYPES ====================

void compressText();
void calculateFrequency();
void buildHuffmanTree();
void generateCodes(Node* node, string code);
void displayFrequencyTable();
void displayHuffmanTree(Node* node, string space, string direction);
void displayCodes();
void compressionStatistics();
void decompressFile();
void verifyCompression();


// ==================== 1. COMPRESS TEXT/FILE ====================

void compressText() {

    int choice;

    cout << "\n1. Enter Text";
    cout << "\n2. Read Text File";
    cout << "\nEnter choice: ";
    cin >> choice;

    cin.ignore();

    if (choice == 1) {

        cout << "\nEnter text: ";
        getline(cin, inputText);

    }
    else if (choice == 2) {

        string filename;

        cout << "\nEnter file name: ";
        getline(cin, filename);

        ifstream file(filename);

        if (!file) {
            cout << "\nFile could not be opened.\n";
            return;
        }

        string line;

        inputText = "";

        while (getline(file, line)) {
            inputText += line;
            inputText += '\n';
        }

        file.close();

    }
    else {
        cout << "\nInvalid choice.\n";
        return;
    }

    if (inputText.empty()) {
        cout << "\nNo input provided.\n";
        return;
    }

    calculateFrequency();
    buildHuffmanTree();

    huffmanCodes.clear();

    generateCodes(root, "");

    encodedText = "";

    for (char c : inputText) {
        encodedText += huffmanCodes[c];
    }

    cout << "\nText compressed successfully!\n";

    cout << "\nEncoded data:\n";
    cout << encodedText << endl;
}


// ==================== 2. FREQUENCY TABLE ====================

void calculateFrequency() {

    // ask
    frequencyTable.clear();

    for (char c : inputText) {
        frequencyTable[c]++;
    }
}


void displayFrequencyTable() {

    if (frequencyTable.empty()) {
        cout << "\nNo frequency table available.\n";
        return;
    }

    cout << "\n========== FREQUENCY TABLE ==========\n";

    cout << left
         << setw(15) << "Character"
         << setw(15) << "Frequency"
         << endl;

    cout << "-------------------------------------\n";

    for (auto pair : frequencyTable) {

        char c = pair.first;

        // ask
        if (c == ' ')
            cout << setw(15) << "[SPACE]";
        else if (c == '\n')
            cout << setw(15) << "[NEWLINE]";
        else
            cout << setw(15) << c;

        cout << setw(15) << pair.second << endl;
    }
}


// ==================== 3. BUILD HUFFMAN TREE ====================
// change
void buildHuffmanTree() {

    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (auto pair : frequencyTable) {

        Node* newNode =
            new Node(pair.first, pair.second);

        pq.push(newNode);
    }

    while (pq.size() > 1) {

        Node* left = pq.top();
        pq.pop();

        Node* right = pq.top();
        pq.pop();

        Node* parent =
            new Node(left, right);

        pq.push(parent);
    }

    root = pq.top();
}


// ==================== 4. GENERATE HUFFMAN CODES ====================

void generateCodes(Node* node, string code) {

    if (node == nullptr)
        return;

    // Leaf node
    if (node->left == nullptr &&
        node->right == nullptr) {

        huffmanCodes[node->ch] = code;
        return;
    }

    generateCodes(node->left, code + "0");

    generateCodes(node->right, code + "1");
}


// ==================== DISPLAY HUFFMAN TREE ====================

void displayHuffmanTree(Node* node,
                        string space,
                        string direction) {

    if (node == nullptr)
        return;

    cout << space;

    if (direction != "")
        cout << direction << " ";

    if (node->left == nullptr &&
        node->right == nullptr) {

        if (node->ch == ' ')
            cout << "[SPACE]";
        else if (node->ch == '\n')
            cout << "[NEWLINE]";
        else
            cout << "'" << node->ch << "'";

        cout << " : " << node->frequency << endl;

    }
    else {

        cout << "[*] : "
             << node->frequency << endl;
    }

    displayHuffmanTree(
        node->left,
        space + "    ",
        "L->"
    );

    displayHuffmanTree(
        node->right,
        space + "    ",
        "R->"
    );
}


// ==================== 5. DISPLAY HUFFMAN CODES ====================

void displayCodes() {

    if (huffmanCodes.empty()) {
        cout << "\nHuffman codes are not available.\n";
        return;
    }

    cout << "\n========== HUFFMAN CODES ==========\n";

    cout << left
         << setw(15) << "Character"
         << setw(15) << "Code"
         << endl;

    cout << "-----------------------------------\n";

    for (auto pair : huffmanCodes) {

        if (pair.first == ' ')
            cout << setw(15) << "[SPACE]";
        else if (pair.first == '\n')
            cout << setw(15) << "[NEWLINE]";
        else
            cout << setw(15) << pair.first;

        cout << setw(15) << pair.second << endl;
    }
}


// ==================== 6. COMPRESSION STATISTICS ====================

void compressionStatistics() {

    if (inputText.empty() ||
        encodedText.empty()) {

        cout << "\nNo compressed data available.\n";
        return;
    }

    int originalBits =
        inputText.length() * 8;

    int compressedBits =
        encodedText.length();

    double compressionRatio =
        (double)compressedBits /
        originalBits;

    double spaceSaved =
        (1 - compressionRatio) * 100;

    cout << "\n========== COMPRESSION STATISTICS ==========\n";

    cout << "Original size       : "
         << originalBits
         << " bits\n";

    cout << "Compressed size     : "
         << compressedBits
         << " bits\n";

    cout << fixed << setprecision(2);

    cout << "Compression ratio   : "
         << compressionRatio
         << endl;

    cout << "Space saved         : "
         << spaceSaved
         << "%\n";
}


// ==================== 7. DECOMPRESS ====================

void decompressFile() {

    if (encodedText.empty() || root == nullptr) {

        cout << "\nNo compressed data available.\n";
        return;
    }

    decodedText = "";

    Node* current = root;

    for (char bit : encodedText) {

        if (bit == '0')
            current = current->left;

        else if (bit == '1')
            current = current->right;

        if (current->left == nullptr &&
            current->right == nullptr) {

            decodedText += current->ch;

            current = root;
        }
    }

    cout << "\n========== DECOMPRESSED TEXT ==========\n";

    cout << decodedText << endl;
}


// ==================== 8. VERIFY LOSSLESS ====================

void verifyCompression() {

    if (inputText.empty() ||
        decodedText.empty()) {

        cout << "\nPlease compress and decompress first.\n";
        return;
    }

    if (inputText == decodedText) {

        cout << "\n====================================\n";
        cout << "LOSSLESS COMPRESSION VERIFIED!\n";
        cout << "Original text and decoded text match.\n";
        cout << "====================================\n";
    }
    else {

        cout << "\nCompression verification FAILED.\n";
    }
}


// ==================== MAIN ====================

int main() {

    int choice;

    do {

        cout << "\n\n";
        cout << "========== HUFFMAN COMPRESSION ==========\n";

        cout << "\n1. Compress Text/File";
        cout << "\n2. View Frequency Table";
        cout << "\n3. View Huffman Tree";
        cout << "\n4. View Huffman Codes";
        cout << "\n5. View Compression Statistics";
        cout << "\n6. Decompress File";
        cout << "\n7. Verify Lossless Compression";
        cout << "\n8. Exit";

        cout << "\n\nEnter your choice: ";
        cin >> choice;

        switch (choice) {

            case 1:
                compressText();
                break;

            case 2:
                displayFrequencyTable();
                break;

            case 3:

                if (root == nullptr)
                    cout << "\nHuffman tree is not available.\n";
                else {

                    cout << "\n========== HUFFMAN TREE ==========\n";

                    displayHuffmanTree(
                        root,
                        "",
                        ""
                    );
                }

                break;

            case 4:
                displayCodes();
                break;

            case 5:
                compressionStatistics();
                break;

            case 6:
                decompressFile();
                break;

            case 7:
                verifyCompression();
                break;

            case 8:
                cout << "\nExiting program...\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }

    } while (choice != 8);

    return 0;
}
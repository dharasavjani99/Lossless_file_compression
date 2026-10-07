#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint> // for unit64_t
#include <iomanip>
using namespace std;

struct Node{
    unsigned char ch;
    uint64_t frequency;
    Node* lc;
    Node* rc;
};

Node* createNode(unsigned char ch, uint64_t frequency){
    Node* t = new Node;
    t->ch = ch;
    t->frequency = frequency;
    t->lc = NULL;
    t->rc = NULL;
    return t;
}

Node* createParent(Node* left, Node* right){
    Node* t = new Node;
    t->ch = '\0';
    t->frequency = left->frequency + right->frequency;
    t->lc = left;
    t->rc = right;
    return t;
}

bool isLeaf(Node* root){
    return root->lc == NULL && root->rc == NULL;
}

int findMinimum(vector<Node*>& nodes){
    int minIndex = 0;
    for(int i = 1; i < nodes.size(); i++){
        if(nodes[i]->frequency < nodes[minIndex]->frequency){
            minIndex = i;
        }
    }
    return minIndex;
}

vector<pair<unsigned char, uint64_t>> buildFrequencyTable(const string& text){
    vector<pair<unsigned char, uint64_t>> frequency;
    for(unsigned char ch : text){
        bool found = false;
        for(auto& p : frequency){
            if(p.first == ch){
                p.second++;
                found = true;
                break;
            }
        }
        if(!found){
            frequency.push_back({ch, 1});
        }
    }
    return frequency;
}

void displayCharacter(unsigned char ch){
    if(ch == ' ')
        cout << "[Space]";
    else if(ch == '\n')
        cout << "[Newline]";
    else if(ch == '\t')
        cout << "[Tab]";
    else
        cout << ch;
}

void displayFrequency(vector<pair<unsigned char, uint64_t>>& frequency){
    cout << "\n";
    cout << "========================================\n";
    cout << "          FREQUENCY TABLE\n";
    cout << "========================================\n";
    cout << left << setw(15) << "Character" << "Frequency\n";
    cout << "----------------------------------------\n";

    for(auto p : frequency){
        cout << left;
        cout << setw(15);
        if(p.first == ' ')
            cout << "[Space]";
        else if(p.first == '\n')
            cout << "[Newline]";
        else if(p.first == '\t')
            cout << "[Tab]";
        else
            cout << p.first;
        cout << p.second << endl;
    }
}

Node* buildHuffmanTree(vector<pair<unsigned char, uint64_t>>& frequency){
    vector<Node*> nodes;

    // Create leaf nodes
    for(auto p : frequency){
        Node* t = createNode(p.first, p.second);
        nodes.push_back(t);
    }

    // Repeatedly combine two minimum nodes
    while(nodes.size() > 1){
        // Find smallest node
        int first = findMinimum(nodes);
        Node* left = nodes[first];

        // Remove smallest node
        nodes.erase(nodes.begin() + first);

        // Find second smallest node
        int second = findMinimum(nodes);
        Node* right = nodes[second];

        // Remove second smallest node
        nodes.erase(nodes.begin() + second);

        // Create parent
        Node* parent =
            createParent(left, right);

        // Put parent back into vector
        nodes.push_back(parent);
    }
    if(nodes.empty())
        return NULL;
    return nodes[0];
}

void displayTree(Node* root, string space = "", string branch = "")
{
    if(root == NULL)
        return;
    cout << space << branch;
    if(isLeaf(root)){
        displayCharacter(root->ch);
        cout << " (" << root->frequency << ")";
    }
    else{
        cout << "* (" << root->frequency << ")";
    }
    cout << endl;
    if(root->lc != NULL){
        displayTree( root->lc, space + "    ", "0 -> " );
    }
    if(root->rc != NULL){
        displayTree( root->rc, space + "    ", "1 -> " );
    }
}

void generateCodes( Node* root, string code, vector<pair<unsigned char, string>>& codes){
    if(root == NULL)
        return;

    // Special case:
    // only one unique character
    if(isLeaf(root)){
        if(code == "")
            code = "0";
        codes.push_back({root->ch, code});
        return;
    }

    // Left side = 0
    generateCodes( root->lc, code + "0", codes );

    // Right side = 1
    generateCodes( root->rc, code + "1", codes );
}

string getCode( unsigned char ch, vector<pair<unsigned char, string>>& codes){
    for(auto p : codes){
        if(p.first == ch)
            return p.second;
    }
    return "";
}

void displayCodes( vector<pair<unsigned char, string>>& codes ) {
    cout << "\n";
    cout << "========================================\n";
    cout << "           HUFFMAN CODES\n";
    cout << "========================================\n";
    cout << left << setw(15) << "Character" << "Code\n";
    cout << "----------------------------------------\n";

    for(auto p : codes){
        cout << left << setw(15);
        if(p.first == ' ')
            cout << "[Space]";
        else if(p.first == '\n')
            cout << "[Newline]";
        else if(p.first == '\t')
            cout << "[Tab]";
        else
            cout << p.first;
        cout << p.second << endl;
    }
}

string encodeText( const string& text, vector<pair<unsigned char, string>>& codes) {
    string encoded = "";
    for(unsigned char ch : text){
        encoded += getCode(ch, codes);
    }
    return encoded;
}

void saveCompressedFile( const string& encoded, vector<pair<unsigned char, uint64_t>>& frequency){
    ofstream file(
        "compressed.huff",
        ios::binary
    );
    if(!file){
        cout << "Error creating compressed file.\n";
        return;
    }

    // Number of unique characters
    uint32_t count = frequency.size();
    file.write(
        reinterpret_cast<char*>(&count),
        sizeof(count)
    );

    // Store character and frequency
    for(auto p : frequency){
        file.write(
            reinterpret_cast<char*>(&p.first),
            sizeof(p.first)
        );
        file.write(
            reinterpret_cast<char*>(&p.second),
            sizeof(p.second)
        );
    }

    // Number of encoded bits
    uint64_t totalBits = encoded.length(); 

    file.write(
        reinterpret_cast<char*>(&totalBits),
        sizeof(totalBits)
    );

    // Convert groups of 8 bits into bytes
    unsigned char byte = 0;
    int bitCount = 0;
    for(char bit : encoded){
        byte <<= 1;
        if(bit == '1')
            byte |= 1;
        bitCount++;
        if(bitCount == 8){
            file.write(
                reinterpret_cast<char*>(&byte),
                sizeof(byte)
            );
            byte = 0;
            bitCount = 0;
        }
    }

    // Remaining bits
    if(bitCount > 0){
        byte <<= (8 - bitCount);
        file.write(
            reinterpret_cast<char*>(&byte),
            sizeof(byte)
        );
    }

    file.close();
}

bool readCompressedFile(vector<pair<unsigned char, uint64_t>>& frequency,string& encoded){
    ifstream file(
        "compressed.huff",
        ios::binary
    );
    if(!file){
        return false;
    }

    // Read number of unique characters
    uint32_t count;
    file.read(
        reinterpret_cast<char*>(&count),
        sizeof(count)
    );
    frequency.clear();

    // Read character-frequency pairs
    for(uint32_t i = 0; i < count; i++){
        unsigned char ch;
        uint64_t freq;
        file.read(
            reinterpret_cast<char*>(&ch),
            sizeof(ch)
        );
        file.read(
            reinterpret_cast<char*>(&freq),
            sizeof(freq)
        );
        frequency.push_back(
            {ch, freq}
        );
    }

    // Read total number of encoded bits
    uint64_t totalBits;

    file.read(
        reinterpret_cast<char*>(&totalBits),
        sizeof(totalBits)
    );

    // Read remaining bytes
    vector<unsigned char> bytes;
    unsigned char byte;
    while(file.read(reinterpret_cast<char*>(&byte), sizeof(byte))){
        bytes.push_back(byte);
    }
    file.close();

    // Convert bytes back into bits
    encoded = "";
    for(unsigned char b : bytes){
        for(int i = 7; i >= 0; i--){
            if(b & (1 << i))
                encoded += '1';
            else
                encoded += '0';

            if(encoded.length() == totalBits)
                break;
        }
        if(encoded.length() == totalBits)
            break;
    }
    return true;
}

string decodeText( const string& encoded, Node* root){
    string decoded = "";
    if(root == NULL)
        return decoded;

    // Only one unique character
    if(isLeaf(root)){
        for(int i = 0; i < encoded.length(); i++){
            decoded += static_cast<char>(root->ch);
        }
        return decoded;
    }
    Node* current = root;
    for(char bit : encoded){
        if(bit == '0')
            current = current->lc;
        else
            current = current->rc;

        if(isLeaf(current)){
            decoded += static_cast<char>(current->ch);
            current = root;
        }
    }
    return decoded;
}

void displayStatistics( const string& original, const string& encoded, const string& filename){
    uint64_t originalBits = original.length() * 8;
    uint64_t compressedDataBits = encoded.length();

    // Actual file size
    ifstream file(
        filename,
        ios::binary | ios::ate
    );
    uint64_t actualFileBytes = 0;
    if(file){
        actualFileBytes = file.tellg();
        file.close();
    }

    cout << "\n";
    cout << "========================================\n";
    cout << "       COMPRESSION STATISTICS\n";
    cout << "========================================\n";

    cout << "Original size      : " << originalBits << " bits\n";
    cout << "Encoded data       : " << compressedDataBits << " bits\n";
    cout << "Actual .huff size  : " << actualFileBytes << " bytes\n";

    if(originalBits > 0){
        double ratio =(double)compressedDataBits / originalBits;
        double saved = (1.0 - ratio) * 100.0;
        cout << fixed << setprecision(2);
        cout << "Compression ratio  : " << ratio << endl;
        cout << "Space saved        : " << saved << "%\n";
    }
}

void deleteTree(Node* root){
    if(root == NULL)
        return;
    deleteTree(root->lc);
    deleteTree(root->rc);
    delete root;
}

void compressText(){
    string inputText;
    string encodedText;
    int choice;
    cout << "\n1. Enter Text";
    cout << "\n2. Read Text File";
    cout << "\nEnter choice: ";
    cin >> choice;
    cin.ignore();
    if(choice == 1){
        cout << "\nEnter text: ";
        getline(cin, inputText);
    }
    else if(choice == 2){
        string filename;
        cout << "\nEnter file name: ";
        getline(cin, filename);
        ifstream file(filename);
        if(!file)
        {
            cout << "\nFile could not be opened.\n";
            return;
        }
        string line;
        inputText = "";
        while(getline(file, line))
        {
            inputText += line;
            inputText += '\n';
        }
        file.close();
        cout << "\nFile read successfully.\n";
    }
    else{
        cout << "\nInvalid choice.\n";
        return;
    }
    if(inputText.empty()){
        cout << "\nNo input provided.\n";
        return;
    }
    vector<pair<unsigned char, uint64_t>> frequency = buildFrequencyTable(inputText);
    displayFrequency(frequency);
    Node* root = buildHuffmanTree(frequency);
    cout << "\n";
    cout << "========================================\n";
    cout << "            HUFFMAN TREE\n";
    cout << "========================================\n";
    displayTree(root);
    vector<pair<unsigned char, string>> codes;
    generateCodes( root, "", codes );
    displayCodes(codes);
    string encoded = encodeText(inputText, codes);

    cout << "\n";
    cout << "========================================\n";
    cout << "             ENCODED DATA\n";
    cout << "========================================\n";
    cout << encoded << endl;
    saveCompressedFile( encoded, frequency );

    displayStatistics(inputText, encoded, "compressed.huff");
    cout << "\nCompressed file created successfully.\n";
    cout << "File name: compressed.huff\n";
}

void decompressText(){
    vector<pair<unsigned char, uint64_t>> frequency;
    string encoded;

    // Read compressed file
    bool success = readCompressedFile(frequency, encoded);
    if(!success){
        cout << "\ncompressed.huff not found.\n";
        return;
    }

    // Rebuild the same Huffman tree
    Node* root = buildHuffmanTree(frequency);

    cout << "\n";
    cout << "========================================\n";
    cout << "        DECOMPRESSION\n";
    cout << "========================================\n";
    cout << "\nCompressed data:\n";
    cout << encoded << endl;

    // Decode
    string decoded = decodeText(encoded, root);
    cout << "\nDecompressed text:\n";
    cout << decoded << endl;
    bool valid = true;

    vector<pair<unsigned char, uint64_t>> decodedFrequency = buildFrequencyTable(decoded);

    if(decodedFrequency.size() != frequency.size()){
        valid = false;
    }
    else{
        for(auto p : frequency){
            bool found = false;
            for(auto q : decodedFrequency){
                if(p.first == q.first && p.second == q.second){
                    found = true;
                    break;
                }
            }
            if(!found){
                valid = false;
                break;
            }
        }
    }

    if(valid){
        cout << "\n";
        cout << "========================================\n";
        cout << "       LOSSLESS VERIFICATION\n";
        cout << "========================================\n";

        cout << "Verification successful!\n";
        cout << "The decompressed data contains exactly\n";
        cout << "the same characters with the same frequencies.\n";
    }
    else{
        cout << "\nLossless verification failed.\n";
    }
    deleteTree(root);
}

int main(){
    int choice;
    do{
        cout << "\n";
        cout << "========================================\n";
        cout << "       HUFFMAN FILE COMPRESSION\n";
        cout << "========================================\n";

        cout << "1. Compress Text/File\n";
        cout << "2. Decompress\n";
        cout << "3. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
                compressText();
                break;
            case 2:
                decompressText();
                break;
            case 3:
                cout << "\nExiting program...\n";
                break;
            default:
                cout << "\nInvalid choice.\n";
        }
    } while(choice != 3);
    return 0;
}
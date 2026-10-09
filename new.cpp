#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;


// ---------------- NODE ----------------

struct Node
{
    char ch;
    int frequency;

    Node* left;
    Node* right;
};


// Create a node
Node* createNode(char ch, int frequency)
{
    Node* node = new Node;

    node->ch = ch;
    node->frequency = frequency;

    node->left = NULL;
    node->right = NULL;

    return node;
}


// Create a parent node
Node* createParent(Node* left, Node* right)
{
    Node* node = new Node;

    node->ch = '\0';
    node->frequency = left->frequency + right->frequency;

    node->left = left;
    node->right = right;

    return node;
}


// Check if node is a leaf
bool isLeaf(Node* node)
{
    return node->left == NULL && node->right == NULL;
}


// ---------------- FREQUENCY ----------------

vector<pair<char, int>> getFrequency(string text)
{
    vector<pair<char, int>> frequency;

    for (char ch : text)
    {
        bool found = false;

        for (auto& p : frequency)
        {
            if (p.first == ch)
            {
                p.second++;
                found = true;
                break;
            }
        }

        if (!found)
        {
            frequency.push_back({ch, 1});
        }
    }

    return frequency;
}


// ---------------- FIND SMALLEST ----------------

int findSmallest(vector<Node*>& nodes)
{
    int smallest = 0;

    for (int i = 1; i < nodes.size(); i++)
    {
        if (nodes[i]->frequency < nodes[smallest]->frequency)
        {
            smallest = i;
        }
    }

    return smallest;
}


// ---------------- BUILD TREE ----------------

Node* buildTree(vector<pair<char, int>>& frequency)
{
    vector<Node*> nodes;

    // Create one node for every character
    for (auto p : frequency)
    {
        nodes.push_back(createNode(p.first, p.second));
    }

    // Keep combining the two smallest nodes
    while (nodes.size() > 1)
    {
        // Find smallest
        int first = findSmallest(nodes);
        Node* left = nodes[first];

        nodes.erase(nodes.begin() + first);


        // Find second smallest
        int second = findSmallest(nodes);
        Node* right = nodes[second];

        nodes.erase(nodes.begin() + second);


        // Combine them
        Node* parent = createParent(left, right);

        // Put parent back
        nodes.push_back(parent);
    }

    if (nodes.empty())
        return NULL;

    return nodes[0];
}

void inorder(Node* root)
{
    if (root == NULL)
        return;

    inorder(root->left);

    if (isLeaf(root))
        cout << root->ch << "(" << root->frequency << ") ";
    else
        cout << "*(" << root->frequency << ") ";

    inorder(root->right);
}

// ---------------- GENERATE CODES ----------------

void generateCodes(
    Node* root,
    string code,
    vector<pair<char, string>>& codes)
{
    if (root == NULL)
        return;


    // We reached a character
    if (isLeaf(root))
    {
        // If there is only one character
        if (code == "")
            code = "0";

        codes.push_back({root->ch, code});

        return;
    }


    // Go left = 0
    generateCodes(root->left, code + "0", codes);


    // Go right = 1
    generateCodes(root->right, code + "1", codes);
}


// ---------------- GET CODE ----------------

string getCode(
    char ch,
    vector<pair<char, string>>& codes)
{
    for (auto p : codes)
    {
        if (p.first == ch)
        {
            return p.second;
        }
    }

    return "";
}




// ---------------- ENCODE ----------------

string encode(
    string text,
    vector<pair<char, string>>& codes)
{
    string result = "";

    for (char ch : text)
    {
        result += getCode(ch, codes);
    }

    return result;
}


// ---------------- DECODE ----------------

string decode(
    string encoded,
    Node* root)
{
    string result = "";

    if (root == NULL)
        return result;


    // Only one unique character
    if (isLeaf(root))
    {
        for (int i = 0; i < encoded.length(); i++)
        {
            result += root->ch;
        }

        return result;
    }


    Node* current = root;

    for (char bit : encoded)
    {
        // 0 = go left
        if (bit == '0')
        {
            current = current->left;
        }

        // 1 = go right
        else
        {
            current = current->right;
        }


        // Reached a character
        if (isLeaf(current))
        {
            result += current->ch;

            // Start again from root
            current = root;
        }
    }

    return result;
}


// ---------------- DISPLAY FREQUENCY ----------------

void showFrequency(vector<pair<char, int>>& frequency)
{
    cout << "\nFrequency Table\n";
    cout << "---------------\n";

    for (auto p : frequency)
    {
        if (p.first == ' ')
            cout << "[Space]";
        else
            cout << p.first;

        cout << " : " << p.second << endl;
    }
}


// ---------------- DISPLAY CODES ----------------

void showCodes(vector<pair<char, string>>& codes)
{
    cout << "\nHuffman Codes\n";
    cout << "-------------\n";

    for (auto p : codes)
    {
        if (p.first == ' ')
            cout << "[Space]";
        else
            cout << p.first;

        cout << " : " << p.second << endl;
    }
}


// ---------------- DELETE TREE ----------------

void deleteTree(Node* root)
{
    if (root == NULL)
        return;

    deleteTree(root->left);
    deleteTree(root->right);

    delete root;
}


// ---------------- MAIN ----------------

int main()
{
    string text;

    cout << "Enter text: ";
    getline(cin, text);


    if (text.empty())
    {
        cout << "No text entered.\n";
        return 0;
    }


    // 1. Find frequency
    vector<pair<char, int>> frequency = getFrequency(text);

    showFrequency(frequency);


    // 2. Build Huffman tree
    Node* root = buildTree(frequency);

    cout << "\nHuffman Tree Traversal: ";
    inorder(root);
    cout << endl;

    // 3. Generate Huffman codes
    vector<pair<char, string>> codes;

    generateCodes(root, "", codes);

    showCodes(codes);


    // 4. Encode the text
    string encoded = encode(text, codes);

    cout << "\nEncoded text:\n";
    cout << encoded << endl;



    // 8. Calculate sizes and compression percentage

    int originalSize = text.length() * 8;
    int compressedSize = encoded.length();

    double compressionPercentage =
        ((double)(originalSize - compressedSize) / originalSize) * 100;

    cout << "\nSize Calculation\n";
    cout << "----------------\n";

    cout << "Original Size: " << originalSize << " bits" << endl;
    cout << "Compressed Size: " << compressedSize << " bits" << endl;

    cout << "Compression Percentage: "
        << compressionPercentage << "%" << endl;


    // 5. Decode the encoded text
    string decoded = decode(encoded, root);

    cout << "\nDecoded text:\n";
    cout << decoded << endl;


    // 6. Check if original and decoded are same
    if (text == decoded)
    {
        cout << "\nCompression is lossless!\n";
    }
    else
    {
        cout << "\nSomething went wrong.\n";
    }


    // 7. Delete tree from memory
    deleteTree(root);

    return 0;
}




// The sun was shining brightly as the birds chirped in the trees. A gentle breeze flowed through the garden, carrying the sweet fragrance of fresh flowers. Children played happily in the park while their parents sat on nearby benches. Some people walked along the peaceful paths, enjoying the beauty of nature. It was a perfect day to relax, spend time with loved ones, and appreciate the simple joys of life. As the evening approached, the sky turned orange and pink, creating a beautiful sunset that filled everyone with a sense of peace and happiness.

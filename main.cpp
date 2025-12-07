#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>

using namespace std;

// Nombre de rptitions pour les tests ultra-rapides (Recherche et Suppression)
#define NUM_REPETITIONS 500000 


// ==================================================================
// 1) TIMER SIMPLE
// ==================================================================
class Timer {
    chrono::high_resolution_clock::time_point startT, endT;
public:
    void start() { startT = chrono::high_resolution_clock::now(); }
    void stop()  { endT = chrono::high_resolution_clock::now(); }

    // Retourne le temps coul en ms sous forme de chane formate
    string elapsed(const string& msg) {
        double ms = chrono::duration<double, milli>(endT - startT).count();
        char buffer[100];
        // 4 dcimales pour l'affichage de l'insertion
        sprintf(buffer, "%.4f", ms); 
        return msg + string(buffer) + " ms\n";
    }
    
    // Retourne le temps coul en double (pour un calcul prcis de la moyenne)
    double get_elapsed_ms() const {
        return chrono::duration<double, milli>(endT - startT).count();
    }
    
    // Retourne le temps MOYEN en ms avec 6 dcimales
    string average_elapsed(const string& msg) const {
        double total_ms = get_elapsed_ms();
        double avg_ms = total_ms / NUM_REPETITIONS;
        char buffer[100];
        // 6 dcimales pour l'affichage du temps moyen (trs court)
        sprintf(buffer, "%.6f", avg_ms); 
        return msg + string(buffer) + " ms \n";
    }
};


// ==================================================================
// 2) RED-BLACK TREE (IMPLEMENTATION PERSONNALISEE)
// ==================================================================
enum Color { RED, BLACK };

struct RBNode {
    char value;
    Color color;
    RBNode *parent;
    RBNode *left;
    RBNode *right;

    RBNode(char val) : value(val), color(RED), parent(nullptr), left(nullptr), right(nullptr) {}
};

class RedBlackTree {
private:
    RBNode *root;
    RBNode *TNULL;

    void leftRotate(RBNode *x) {
        RBNode *y = x->right;
        x->right = y->left;
        if (y->left != TNULL) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rightRotate(RBNode *x) {
        RBNode *y = x->left;
        x->left = y->right;
        if (y->right != TNULL) y->right->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr) root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void fixInsert(RBNode *k) {
        RBNode *u;
        while (k->parent && k->parent->parent && k->parent->color == RED) {
            if (k->parent == k->parent->parent->right) {
                u = k->parent->parent->left;
                if (u && u->color == RED) {
                    u->color = BLACK;
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->left) {
                        k = k->parent;
                        rightRotate(k);
                    }
                    if (k->parent) k->parent->color = BLACK;
                    if (k->parent && k->parent->parent) k->parent->parent->color = RED;
                    if (k->parent && k->parent->parent) leftRotate(k->parent->parent);
                }
            } else {
                u = k->parent->parent->right;
                if (u && u->color == RED) {
                    u->color = BLACK;
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->right) {
                        k = k->parent;
                        leftRotate(k);
                    }
                    if (k->parent) k->parent->color = BLACK;
                    if (k->parent && k->parent->parent) k->parent->parent->color = RED;
                    if (k->parent && k->parent->parent) rightRotate(k->parent->parent);
                }
            }
            if (k == root || !k->parent || !k->parent->parent || k->parent->color == BLACK) break;
        }
        if (root) root->color = BLACK;
    }


    RBNode *searchTreeHelper(RBNode *node, char key) {
        if (node == TNULL || key == node->value) return node;
        if (key < node->value) return searchTreeHelper(node->left, key);
        return searchTreeHelper(node->right, key);
    }

    RBNode* minimum(RBNode* node) {
        while (node->left != TNULL) node = node->left;
        return node;
    }

    void rbTransplant(RBNode* u, RBNode* v) {
        if (u->parent == nullptr) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        v->parent = u->parent;
    }

    void fixDelete(RBNode *x) {
        RBNode *w;
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

    void deleteNodeHelper(RBNode* z) {
        RBNode* y = z;
        RBNode* x;
        Color yOriginalColor = y->color;

        if (z->left == TNULL) {
            x = z->right;
            rbTransplant(z, z->right);
        } else if (z->right == TNULL) {
            x = z->left;
            rbTransplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                rbTransplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            rbTransplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        if (x != TNULL && yOriginalColor == BLACK) fixDelete(x);
    }

public:
    RedBlackTree() {
        TNULL = new RBNode('\0');
        TNULL->color = BLACK;
        TNULL->left = TNULL;
        TNULL->right = TNULL;
        root = TNULL;
    }

    ~RedBlackTree() {
        delete TNULL;
    }

    void insert(char key) {
        RBNode *node = new RBNode(key);
        node->left = TNULL;
        node->right = TNULL;

        RBNode *y = nullptr;
        RBNode *x = this->root;

        while (x != TNULL) {
            y = x;
            if (node->value < x->value) x = x->left;
            else if (node->value == x->value) { 
                 delete node;
                 return;
            }
            else x = x->right;
        }

        node->parent = y;
        if (y == nullptr) root = node;
        else if (node->value < y->value) y->left = node;
        else y->right = node;

        if (node->parent == nullptr) {
            node->color = BLACK;
            return;
        }
        if (node->parent->parent == nullptr) return;

        fixInsert(node);
    }

    bool search(char k) {
        return searchTreeHelper(this->root, k) != TNULL;
    }

    bool remove(char k) {
        RBNode *z = searchTreeHelper(this->root, k);
        if (z == TNULL) return false;
        deleteNodeHelper(z);
        return true;
    }
};


// ==================================================================
// 3) ARBRE BINAIRE SIMPLE
// ==================================================================
template<typename T>
class BinaryTree {
private:
    struct Node {
        char key;
        T value;
        Node* left;
        Node* right;
        Node(char k, T v) : key(k), value(v), left(nullptr), right(nullptr) {}
    };

    Node* root = nullptr;

    Node* insert(Node* r, char key, T value) {
        if (!r) return new Node(key, value);
        if (key < r->key) r->left  = insert(r->left, key, value);
        else if (key > r->key) r->right = insert(r->right, key, value);
        return r;
    }

    bool find(Node* r, char key) {
        if (!r) return false;
        if (key == r->key) return true;
        if (key < r->key)  return find(r->left, key);
        return find(r->right, key);
    }

    Node* minValue(Node* r) {
        while (r->left) r = r->left;
        return r;
    }

    Node* removeNode(Node* r, char key) {
        if (!r) return nullptr;

        if (key < r->key) r->left = removeNode(r->left, key);
        else if (key > r->key) r->right = removeNode(r->right, key);
        else {
            if (!r->left)  { Node* t = r->right; delete r; return t; }
            if (!r->right) { Node* t = r->left;  delete r; return t; }

            Node* temp = minValue(r->right);
            r->key = temp->key;
            r->value = temp->value;
            r->right = removeNode(r->right, temp->key);
        }
        return r;
    }

public:
    void insert(char k, T v) { root = insert(root, k, v); }
    bool search(char k)      { return find(root, k); }
    bool is_present(char k)  { return find(root, k); } // Ajout pour le test de suppression
    void removeKey(char k)   { root = removeNode(root, k); }
};


// ==================================================================
// PROGRAMME PRINCIPAL
// ==================================================================
int main(int argc, char* argv[])
{
    // Fixer la prcision de l'affichage globalement
    cout << fixed << setprecision(4); 
    
    size_t COUNT = 100000000;
    string filename = "test.txt";

    // Arguments en ligne de commande
    if (argc > 1) filename = argv[1];
    if (argc > 2) COUNT = atoi(argv[2]);

    srand(static_cast<unsigned>(time(nullptr)));
    Timer timer;

    cout << "\n+================================================+\n";
    cout << "|   COMPARAISON DE STRUCTURES DE DONNEES         |\n";
    cout << "|   Test avec des caracteres individuels         |\n";
    cout << "+================================================+\n\n";

    // --------------------------------------
    // LECTURE DU FICHIER
    // --------------------------------------
    cout << ">> Lecture du fichier : " << filename << " ...";

    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cout << "\n Impossible d'ouvrir " << filename << "\n";
        return 1;
    }

    vector<char> chars;
    chars.reserve(COUNT);

    char c;
    size_t totalCharsRead = 0;

    // Lire TOUS les caractres sans filtre
    while (chars.size() < COUNT && inputFile.get(c)) {
        chars.push_back(c);  // Garde TOUT
        totalCharsRead++;
    }

    inputFile.close();

    if (chars.empty()) {
        cout << "\n Aucun caractere lu dans le fichier.\n";
        return 1;
    }

    size_t realCount = chars.size();
    cout << " OK\n";
    cout << ">> Nombre TOTAL de caracteres lus : " << realCount << "\n";

    // Afficher si on a atteint la limite ou la fin du fichier
    if (realCount >= COUNT) {
        cout << ">> ATTENTION: Limite COUNT atteinte, fichier peut contenir plus de donnees\n";
    } else {
        cout << ">> Fin du fichier atteinte\n";
    }
    cout << "\n";

    // Caractres de test
    char testChar = chars[realCount / 2];
    char delChar  = chars[realCount / 3]; 

    cout << ">> Caractere de test (recherche) : '" << testChar << "' (code ASCII: " << (int)testChar << ")\n";
    cout << ">> Caractere de test (suppression) : '" << delChar << "' (code ASCII: " << (int)delChar << ")\n\n";
    cout << ">> Recherche et Suppression mesures sur " << NUM_REPETITIONS << " rptitions.\n\n";

    // Variable pour stocker la valeur  r-insrer pour les map/BST
    int delCharValue = 0; 
    
    
    // ==================================================================
    // TEST 1 : RED-BLACK TREE (Implementation personnalise)
    // ==================================================================
    cout << "+--------------------------------------------------+\n";
    cout << " TEST 1 : Red-Black Tree (Implementation Custom)  \n\n\n";

    RedBlackTree rbt;

    cout << " > Insertion de " << realCount << " caracteres...";
    timer.start();
    for (char ch : chars) rbt.insert(ch);
    timer.stop();
    cout << "\n  " << timer.elapsed("Temps : ");

    // --- RECHERCHE RPTE ---
    cout << " > Recherche du caractere '" << testChar << "'...";
    timer.start();
    bool found1 = false;
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        found1 = rbt.search(testChar); 
    }
    timer.stop();
    
    cout << "\n  Resultat : " << (found1 ? "Trouve" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");

    // --- SUPPRESSION RPTE ---
    cout << " > Suppression du caractere '" << delChar << "'...";
    timer.start();
    bool removed1 = false;
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        removed1 = rbt.remove(delChar);
        if (removed1) rbt.insert(delChar); // R-insrer pour maintenir l'tat
    }
    timer.stop();

    cout << "\n  Resultat : " << (removed1 ? "Supprime" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // ==================================================================
    // TEST 2 : std::set
    // ==================================================================
    cout << "\n\n+--------------------------------------------------+\n";
    cout << " TEST 2 : std::set<char> (Red-Black Tree STL)     \n\n\n";

    set<char> stlSet;

    cout << " > Insertion de " << realCount << " caracteres...";
    timer.start();
    for (char ch : chars) stlSet.insert(ch);
    timer.stop();
    cout << "\n  " << timer.elapsed("Temps : ");

    // --- RECHERCHE RPTE ---
    cout << " > Recherche du caractere '" << testChar << "'...";
    timer.start();
    auto it2 = stlSet.end();
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        it2 = stlSet.find(testChar);
    }
    timer.stop();

    cout << "\n  Resultat : " << (it2 != stlSet.end() ? "Trouve" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // --- SUPPRESSION RPTE ---
    cout << " > Suppression du caractere '" << delChar << "'...";
    timer.start();
    size_t erased2 = 0;
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        erased2 = stlSet.erase(delChar);
        if (erased2 > 0) stlSet.insert(delChar); // R-insrer pour maintenir l'tat
    }
    timer.stop();

    cout << "\n  Resultat : " << (erased2 > 0 ? "Supprime" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // ==================================================================
    // TEST 3 : std::unordered_set
    // ==================================================================
    cout << "\n\n+--------------------------------------------------+\n";
    cout << " TEST 3 : std::unordered_set<char> (Hash Table)   \n\n\n";

    unordered_set<char> uSet;

    cout << " > Insertion de " << realCount << " caracteres...";
    timer.start();
    for (char ch : chars) uSet.insert(ch);
    timer.stop();
    cout << "\n  " << timer.elapsed("Temps : ");

    // --- RECHERCHE RPTE ---
    cout << " > Recherche du caractere '" << testChar << "'...";
    timer.start();
    auto it3 = uSet.end();
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        it3 = uSet.find(testChar);
    }
    timer.stop();

    cout << "\n  Resultat : " << (it3 != uSet.end() ? "Trouve" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // --- SUPPRESSION RPTE ---
    cout << " > Suppression du caractere '" << delChar << "'...";
    timer.start();
    size_t erased3 = 0;
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        erased3 = uSet.erase(delChar);
        if (erased3 > 0) uSet.insert(delChar); // R-insrer pour maintenir l'tat
    }
    timer.stop();

    cout << "\n  Resultat : " << (erased3 > 0 ? "Supprime" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // ==================================================================
    // TEST 4 : std::map
    // ==================================================================
    cout << "\n\n+--------------------------------------------------+\n";
    cout << " TEST 4 : std::map<char, int> (Red-Black Tree)    \n\n\n";

    map<char, int> stlMap;

    cout << " > Insertion de " << realCount << " caracteres...";
    timer.start();
    for (size_t i = 0; i < realCount; ++i) {
        stlMap[chars[i]] = i;
    }
    timer.stop();
    cout << "\n  " << timer.elapsed("Temps : ");
    
    // Rcuprer la valeur  r-insrer pour la map
    auto it_del_map = stlMap.find(delChar);
    if (it_del_map != stlMap.end()) delCharValue = it_del_map->second;
    else delCharValue = -1; 
    
    // --- RECHERCHE RPTE ---
    cout << " > Recherche du caractere '" << testChar << "'...";
    timer.start();
    auto it4 = stlMap.end();
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        it4 = stlMap.find(testChar);
    }
    timer.stop();

    cout << "\n  Resultat : " << (it4 != stlMap.end() ? "Trouve" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // --- SUPPRESSION RPTE ---
    cout << " > Suppression du caractere '" << delChar << "'...";
    timer.start();
    size_t erased4 = 0;
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        erased4 = stlMap.erase(delChar);
        if (erased4 > 0 && delCharValue != -1) stlMap[delChar] = delCharValue; // R-insrer pour maintenir l'tat
    }
    timer.stop();

    cout << "\n  Resultat : " << (erased4 > 0 ? "Supprime" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // ==================================================================
    // TEST 5 : std::unordered_map
    // ==================================================================
    cout << "\n\n+--------------------------------------------------+\n";
    cout << " TEST 5 : std::unordered_map<char, int>           \n\n\n";

    unordered_map<char, int> uMap;

    cout << " > Insertion de " << realCount << " caracteres...";
    timer.start();
    for (size_t i = 0; i < realCount; ++i) {
        uMap[chars[i]] = i;
    }
    timer.stop();
    cout << "\n  " << timer.elapsed("Temps : ");
    
    // --- RECHERCHE RPTE ---
    cout << " > Recherche du caractere '" << testChar << "'...";
    timer.start();
    auto it5 = uMap.end();
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        it5 = uMap.find(testChar);
    }
    timer.stop();

    cout << "\n  Resultat : " << (it5 != uMap.end() ? "Trouve" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // --- SUPPRESSION RPTE ---
    cout << " > Suppression du caractere '" << delChar << "'...";
    timer.start();
    size_t erased5 = 0;
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        erased5 = uMap.erase(delChar);
        if (erased5 > 0 && delCharValue != -1) uMap[delChar] = delCharValue; // R-insrer pour maintenir l'tat
    }
    timer.stop();

    cout << "\n  Resultat : " << (erased5 > 0 ? "Supprime" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // ==================================================================
    // TEST 6 : Arbre Binaire Simple
    // ==================================================================
    cout << "\n\n+--------------------------------------------------+\n";
    cout << " TEST 6 : Arbre Binaire Simple (BST)              \n\n\n";

    BinaryTree<int> bt;

    cout << " > Insertion de " << realCount << " caracteres...";
    timer.start();
    for (size_t i = 0; i < realCount; ++i) {
        bt.insert(chars[i], i);
    }
    timer.stop();
    cout << "\n  " << timer.elapsed("Temps : ");
    
    // --- RECHERCHE RPTE ---
    cout << " > Recherche du caractere '" << testChar << "'...";
    timer.start();
    bool found6 = false;
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        found6 = bt.search(testChar);
    }
    timer.stop();

    cout << "\n  Resultat : " << (found6 ? "Trouve" : "Non trouve");
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // --- SUPPRESSION RPTE ---
    // La suppression dans le BST est spciale (pas de boolen retourn)
    cout << " > Suppression du caractere '" << delChar << "'...";
    timer.start();
    for (int i = 0; i < NUM_REPETITIONS; ++i) {
        bool present = bt.is_present(delChar); // Vrifier s'il est l avant de supprimer
        if (present) { 
            bt.removeKey(delChar);
            if (delCharValue != -1) bt.insert(delChar, delCharValue); // R-insrer pour l'tape suivante
        }
    }
    timer.stop();

    cout << "\n  Resultat : Supprime (si trouv)";
    cout << "\n  " << timer.average_elapsed("Temps : ");


    // ==================================================================
    // STATISTIQUES FINALES
    // ==================================================================
    cout << "\n\n+================================================+\n";
    cout << "             STATISTIQUES FINALES               \n\n\n";

    cout << " Caracteres TOTAUX lus        : " << realCount << "\n";
    cout << " Caracteres uniques dans std::set  : " << stlSet.size() << "\n";
    cout << " Caracteres uniques dans unordered_set : " << uSet.size() << "\n";
    cout << " Caracteres uniques dans map     : " << stlMap.size() << "\n";
    cout << " Caracteres uniques dans unordered_map : " << uMap.size() << "\n";

    cout << "\n+================================================+\n";
    cout << "|              BENCHMARK TERMINE                 |\n";
    cout << "+================================================+\n\n";

    return 0;
}

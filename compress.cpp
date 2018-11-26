#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cstring>
#include <string>
#include <cstdio>

using namespace std;

struct HCNode
{
    char data;
    int weight;
    HCNode *lchild;
    HCNode *rchild;
};

struct LLNode
{
    HCNode *data;
    LLNode *next;
    LLNode(HCNode *x, LLNode *n);
};

LLNode::LLNode(HCNode *x, LLNode *n)
{
    data = x;
    next = n;
}

class Stack
{
private:
    int size;
    int top;
    HCNode **data;
public:
    Stack();
    void push(HCNode *x);
    HCNode *pop();
    HCNode *Top();
    int Size();
    bool isEmpty();
};

Stack::Stack()
{
    size = 64;
    top = -1;
    data = new HCNode *[64];
}

void Stack::push(HCNode *x)
{
    if (size == top + 1)
    {
        HCNode **t = new HCNode *[2 * size];
        for (int i = 0; i < size; i++)
            t[i] = data[i];
        size *= 2;
        delete[] data;
        data = t;
    }
    data[++top] = x;
}

HCNode *Stack::pop()
{
    if (top == -1)
    {
        cout << "Stack is empty\n";
        return 0;
    }
    else return data[top--];
}

HCNode *Stack::Top()
{
    if (top == -1)
    {
        cout << "Stack is empty\n";
        return 0;
    }
    else return data[top];
}

int Stack::Size()
{
    return top + 1;
}

bool Stack::isEmpty()
{
    return top == -1;
}

void insert(LLNode *&L, HCNode *x)
{
    LLNode *c = L;
    if (L == NULL)
        L = new LLNode(x, NULL);
    else if (L->data->weight > x->weight)
    {
        L = new LLNode(x, L);
    }
    else
    {
        while (c->next != NULL && c->next->data->weight < x->weight)
            c = c->next;
        c->next = new LLNode(x, c->next);
    }
}

int length(LLNode *T)
{
    if (T == NULL)
        return 0;
    else return length(T->next) + 1;
}

bool isLeaf(HCNode *T)
{
    if (T->lchild == NULL && T->rchild == NULL)
        return true;
    else return false;
}

HCNode *build(vector<char> a, vector<int> freq)
{
    LLNode *L = NULL;
    for (int i = 0; i < a.size(); i++)
    {
        HCNode *t = new HCNode;
        t->data = a[i];
        t->weight = freq[i];
        t->lchild = NULL;
        t->rchild = NULL;
        insert(L, t);
    }
    while (length(L) > 1)
    {
        LLNode *l1, *l2;
        l1 = L;
        l2 = L->next;
        L = L->next->next;
        l1->next = NULL;
        l2->next = NULL;
        HCNode *t1, *t2;
        t1 = l1->data;
        t2 = l2->data;
        delete l1;
        delete l2;
        HCNode *t = new HCNode;
        t->data = '0';
        t->weight = t1->weight + t2->weight;
        t->lchild = t1;
        t->rchild = t2;
        insert(L, t);
    }
    return L->data;
}

void treetosequence(HCNode *root, ofstream &fout)
{
    Stack st;
    st.push(root);
    fout << root->data;
    char state = 'l';
    while (!st.isEmpty())
    {
        if (state == 'l')
        {
            if (st.Top()->lchild != NULL)
            {
                st.push(st.Top()->lchild);
                fout << st.Top()->data;
            }
            else
            {
                fout << '\\';
                state = 'r';
            }
        }
        else
        {
            if (st.Top()->rchild != NULL)
            {
                st.push(st.Top()->rchild);
                fout << st.Top()->data;
                state = 'l';
            }
            else
            {
                fout << '\\';
                state = 'r';
                HCNode *k = st.pop();
                while (!st.isEmpty() && st.Top()->rchild == k)
                {
                    k = st.pop();
                }
            }
        }
    }
    fout << "/";
}

void fill(HCNode *T, map<char, string> &code, string s = "")
{
    if(isLeaf(T))
        code.insert(pair<char, string>(T->data, s));
    else
    {
        fill(T->lchild, code, s + '0');
        fill(T->rchild, code, s + '1');
    }
}
int pow(int a, int b)
{
    int ans = 1;
    while(b--)
        ans *= a;
    return ans;
}

int main(int argc, char **argv)
{
    if(argc == 1)
    {
        cout << "Enter file to compress\n";
        return 0;
    }
    string input = string(argv[1]);
    ifstream fin;
    fin.open(input);
    if(fin.fail())
    {
        cout << "Invalid file name\n";
        return 0;
    }
    ofstream fout;
    string output;
    string routput;
    for(int i = 0; i < input.length(); i++)
    {
        if(input[i] == '.')
        {
            output += "_compressed.hzip";
            break;
        }
        output += input[i];
    }
    routput = output;
    output += "1";
    fout.open(output);
    int freq[256] = { 0 };
    char c;
    while(fin.get(c))
    {
        freq[c]++;
    }
    vector<char> a;
    vector<int> f;
    for(int i = 0; i < 256; i++)
    {
        if(freq[i])
        {
            a.push_back(i);
            f.push_back(freq[i]);
        }
    }
    HCNode *T = build(a, f);
    treetosequence(T, fout);
    fin.close();
    fin.open(input);
    map<char, string> code;
    fill(T, code);
    int buffer[8];
    int b = 0;
    while(fin.get(c))
    {
        string ch = code[c];
        int i = 0;
        while(i < ch.length())
        {
            buffer[b++] = ch[i++] - 48;
            if(b == 8)
            {
                int opc = 0;
                for(int j = 0; j < 8; j++)
                    opc += buffer[7 - j] * pow(2, j);
                fout.put((char)opc);
                b = 0;
            }
        }
    }
    int ext = 0;
    if(b != 0)
    {
        while(b < 8)
        {
            buffer[b++] = 0;
            ext++;
        }
        int opc = 0;
        for(int i = 0; i < 8; i++)
        {
            opc += buffer[7 - i] * pow(2, i);
        }
        fout.put((char)opc);
        b = 0;
    }
    fout.close();
    fin.close();
    fout.open(routput);
    fin.open(output);
    fout << ext << " ";
    while(fin.get(c))
    {
        fout.put(c);
    }
    remove(output.c_str());
    cout << "Compression done!\n\n";
    return 0;
}

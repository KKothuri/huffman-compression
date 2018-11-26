#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

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

class iStack
{
private:
    int size;
    int top;
    int *data;
public:
    iStack();
    void push(int x);
    int pop();
    int Top();
    int Size();
    bool isEmpty();
};

iStack::iStack()
{
    size = 64;
    top = -1;
    data = new int[64];
}

void iStack::push(int x)
{
    if (size == top + 1)
    {
        int *t = new int [2 * size];
        for (int i = 0; i < size; i++)
            t[i] = data[i];
        size *= 2;
        delete[] data;
        data = t;
    }
    data[++top] = x;
}

int iStack::pop()
{
    if (top == -1)
    {
        cout << "Stack is empty\n";
        return 0;
    }
    else return data[top--];
}

int iStack::Top()
{
    if (top == -1)
    {
        cout << "Stack is empty\n";
        return 0;
    }
    else return data[top];
}

int iStack::Size()
{
    return top + 1;
}

bool iStack::isEmpty()
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

HCNode *seqtotree(ifstream &fin)
{
    char c;
    fin.get(c);
    HCNode *root = new HCNode;
    root->data = c;
    root->lchild = NULL;
    root->rchild = NULL;
    Stack st;
    st.push(root);
    char state = 'l';
    fin.get(c);
    for (int i = 1; c != '/' && !st.isEmpty(); i++)
    {
        if (c != '\\')
        {
            if (state == 'l')
            {
                st.Top()->lchild = new HCNode;
                st.Top()->lchild->data = c;
                st.Top()->lchild->lchild = NULL;
                st.Top()->lchild->rchild = NULL;
                st.push(st.Top()->lchild);
            }
            else
            {
                st.Top()->rchild = new HCNode;
                st.Top()->rchild->data = c;
                st.Top()->rchild->lchild = NULL;
                st.Top()->rchild->rchild = NULL;
                st.push(st.Top()->rchild);
                state = 'l';
            }
        }
        else
        {
            if (state == 'l')
                state = 'r';
            else
            {
                st.pop();
                while (!st.isEmpty() && st.Top()->rchild != NULL)
                    st.pop();
            }
        }
        fin.get(c);
    }
    return root;
}

int main(int argc, char **argv)
{
    if(argc == 1)
    {
        cout << "Enter file to decompress\n";
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
    for(int i = 0; i < input.length(); i++)
    {
        if(input[i] == '.')
        {
            output += "_decompressed.txt";
            break;
        }
        output += input[i];
    }
    fout.open(output);
    int ext;
    fin >> ext;
    char k;
    fin.get(k);
    HCNode *T = seqtotree(fin);
    fin.close();
    fin.open(input);
    fin.get(k);
    while(k != '/')
    {
        fin.get(k);
    }
    iStack buffer;
    HCNode *t = T;
    char k2;
    fin.get(k2);
    fin.get(k);
    while(!fin.eof())
    {
        int c = k2;
        if(c < 0)
        {
            c += 256;
        }
        for(int i = 0; i < 8; i++)
        {
            buffer.push(c % 2);
            c /= 2;
        }
        while(!buffer.isEmpty())
        {
            if(buffer.pop() == 1)
                t = t->rchild;
            else t = t->lchild;
            if(isLeaf(t))
            {
                fout << t->data;
                t = T;
            }
        }
        k2 = k;
        fin.get(k);
    }
    int c = k2;
    if(c < 0)
    {
        c += 256;
    }
    for(int i = 0; i < 8; i++)
    {
        buffer.push(c % 2);
        c /= 2;
    }
    ext = 8 - ext;
    while(ext--)
    {
        if(buffer.pop() == 1)
            t = t->rchild;
        else t = t->lchild;
        if(isLeaf(t))
        {
            fout << t->data;
            t = T;
        }
    }
    cout << "Decompression done!\n\n";
}

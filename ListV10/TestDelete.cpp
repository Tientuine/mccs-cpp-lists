
int main()
{
    int* p = new int(5);
    int* q = p;

    if (p) { delete p; }
    if (q) { delete q; }

    return 0;
}


int sum(int n)
{
    int result = 0;
    for (int i = 0; i < n; i++)
    {
        result += i;
    }
    return result;
}

int main(void)
{
    return sum(5);
}
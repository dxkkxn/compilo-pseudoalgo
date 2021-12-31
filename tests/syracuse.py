import sys
def syracuse(n):
    iteration = 0
    while (n!= 1 and n!=0):
        if (n%2) == 0:
            n = n/2
        else:
            n = n * 3 + 1
        iteration += 1
        print(n)
    return iteration

if __name__ == "__main__" :
    print(syracuse(int(sys.argv[1])))

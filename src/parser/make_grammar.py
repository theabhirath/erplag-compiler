with open('grammar.csv', 'r') as f:
    with open('arrowed_grammar.txt', 'w') as f1:
        i = 0
        for line in f:
            i += 1
            f1.write(str(i) + ': ')
            first = 0
            for c in line:
                if c == ',' and first == 0:
                    f1.write(' -> ')
                    first = 1
                elif c == ',':
                    f1.write(' ')
                else:
                    f1.write(c)

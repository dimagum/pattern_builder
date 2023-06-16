#include <iostream>
#include <random>
#include <fstream>


int random(int min, int max) {
    static std::mt19937 prng(std::random_device{}());
    return std::uniform_int_distribution<>(min, max)(prng);
}


// класс поля - в нём собираем всё поле
class Field {
    std::string field;
public:
    Field() {
        field = "";
    }
    std::string get_field() {
        return field;
    }
    void add_cell(char symbol) {
        field += symbol;
    }
    void set_key(std::pair<int, int> pos, int n, int m) {
        field[pos.first * m + pos.second] = 'K';
    }
};


class ICellBuilder {
public:
    virtual ~ICellBuilder() = default;
    virtual void buildCell() = 0;
};


class BorderCellBuilder : public ICellBuilder {
    Field* field;

public:
    BorderCellBuilder(Field* field) : field(field) {}

    void buildCell() override {
        field->add_cell('#');
    }
};

class InnerAreaCellBuilder : public ICellBuilder {
    Field* field;

public:
    InnerAreaCellBuilder(Field* field) : field(field) {}

    void buildCell() override {
        field->add_cell(random(0, 10) >= 9 ? '*' : ' ');
    }
};

class DoorCellBuilder : public ICellBuilder {
    Field* field;

public:
    DoorCellBuilder(Field* field) : field(field) {}

    void buildCell() override {
        field->add_cell('0');
    }
};

class NewlineBuilder : public ICellBuilder {
    Field* field;

public:
    NewlineBuilder(Field* field) : field(field) {}

    void buildCell() override {
        field->add_cell('\n');
    }
};


class FieldBuilder {
    Field* field;

    ICellBuilder* borderBuilder;
    ICellBuilder* innerBuilder;
    ICellBuilder* doorBuilder;
    ICellBuilder* newlineBuilder;
    int n, m;

    bool isBorder(int i, int j) {
        return i == 0 || i == n - 1 || j == 0 || j == m - 1;
    }

    bool isCorner(int i, int j) {
        if (i == 0 && j == 0 ||
            i == n - 1 && j == 0 ||
            i == n - 1 && j == m - 1 ||
            i == 0 && j == m - 1)
        {
            return true;
        }
        return false;
    }

    std::pair<int, int> chooseKeyPosition() {
        return std::make_pair(random(1, n - 1 - 1), random(1, m - 1 - 1));
    }

public:
    FieldBuilder(int n, int m) : n(n), m(m) {
        field = new Field;

        borderBuilder = new BorderCellBuilder(field);
        innerBuilder = new InnerAreaCellBuilder(field);
        doorBuilder = new DoorCellBuilder(field);
        newlineBuilder = new NewlineBuilder(field);
    }

    ~FieldBuilder() {
        delete borderBuilder;
        delete innerBuilder;
        delete doorBuilder;
        delete newlineBuilder;

        delete field;
    }

    void set_size(int n, int m) {
        this->n = n;
        this->m = m;
    }

    Field* create_field() {
        int door_counter = 0;

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (isCorner(i, j)) {
                    borderBuilder->buildCell();
                    continue;
                }

                if (isBorder(i, j)) {
                    if (door_counter < 4) {
                        if (random(0, 100) >= 95) {
                            doorBuilder->buildCell();
                            door_counter++;
                        }
                        else {
                            borderBuilder->buildCell();
                        }
                    }
                    else {
                        borderBuilder->buildCell();
                    }
                    continue;
                }
                innerBuilder->buildCell();
            }
            newlineBuilder->buildCell();
        }

        field->set_key(chooseKeyPosition(), n, m);

        return field;
    }

};


int main() {
    Field* field;

    FieldBuilder builder(20, 40);

    field = builder.create_field();

    std::ofstream file("field.txt");

    file << field->get_field();

    file.close();

    std::cout << field->get_field();

    delete field;

    return 0;
}

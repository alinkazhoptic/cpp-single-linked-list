#pragma once

/*
СПРИНТ 6 - Односвязный список
Урок 2 Как устроен односвязный список
Задание 
Создайте шаблонный класс односвязного списка SingleLinkedList<Type> со следующим функционалом:
- конструктор по умолчанию, который создаёт пустой список;
- метод GetSize, который возвращает количество элементов в списке;
- метод IsEmpty, который возвращает true, если список пустой, и false в противном случае.

Урок 3 Вставка элементов и очистка списка
Задание
Реализуйте операцию PushFront, которая делает вставки элемента в начало односвязного списка, и операцию Clear, очищающую список.
Метод PushFront должен предоставлять строгую гарантию безопасности исключений: 
если в процессе работы метода будет выброшено исключение, состояние списка должно быть таким же, как до вызова метода.
Метод Clear очищает список и не должен выбрасывать исключений. Считается, что деструктор типа Type не выбрасывает исключений.
При разрушении списка должны быть удалены все его элементы. 

Урок 5. Сравнение, копирование, присваивание:
Реализуйте в классе односвязного списка следующий функционал:
- Операции сравнения ==, !=, <, >, <=, >=;
- Обмен содержимого двух списков с использованием метода swap и шаблонной функции swap;
- Конструирование односвязного списка на основе initializer_list. 
Последовательность элементов созданного списка и initializer_list должна быть одинаковой;
- Надёжные конструктор копирования и операцию присваивания. 
Операция присваивания должна обеспечивать строгую гарантию безопасности исключений. Если в процессе присваивания 
будет выброшено исключение, содержимое левого аргумента операции присваивания должно остаться без изменений.


Урок 6. Вставка и удаление элемента:
Реализуйте в классе односвязного списка следующие операции:
- Метод PopFront. Удаляет первый элемента непустого списка. Не выбрасывает исключений.
- Метод InsertAfter. Вставляет в список новое значение следом за элементом, на который ссылается 
переданный в InsertAfter итератор. Метод должен обеспечивать строгую гарантию безопасности исключений.
- Метод EraseAfter. Удаляет из списка элемент, следующий за элементом, на который ссылается переданный в EraseAfter итератор. 
Не выбрасывает исключений.
- Методы before_begin и cbefore_begin. Возвращают итераторы, ссылающиеся на фиктивную позицию перед первым элементом списка. 
Такой итератор используется как параметр для методов InsertAfter и EraseAfter, когда нужно вставить или удалить элемент 
в начале списка. Разыменовывать этот итератор нельзя.

*/

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {

    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value{};  
        Node* next_node = nullptr;
    };

    // Шаблон класса «Базовый Итератор».
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node) {
            node_ = node;
        }

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept {
            auto other_copy(other.node_);
            node_ = other_copy;                      

        }

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return (this->node_ == rhs.node_);
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return (this->node_ != rhs.node_);
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return (this->node_ == rhs.node_);
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return (this->node_ != rhs.node_);
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept {
            *this = BasicIterator(this->node_->next_node); // сдвигаем
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        BasicIterator operator++(int) noexcept {
            BasicIterator old_iterator(*this);
            *this = BasicIterator(this->node_->next_node); // сдвигаем
            return old_iterator;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept {
            return (this->node_->value);
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept {
            return &(this->node_->value);
        }



    private:
        Node* node_ = nullptr;
    };



public:

    SingleLinkedList() {};

    ~SingleLinkedList() {
        Clear();
    }

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept {
        Iterator it_begin(head_.next_node);
        return it_begin;
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept {
        Iterator it = begin();
        for (int i = 0; i < size_; i++){
            ++it;
        }
        return it;
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept {
        ConstIterator it_begin(head_.next_node);
        return it_begin;
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept {
        auto it = begin();
        for (int i = 0; i < size_; i++){
            ++it;
        }
        ConstIterator it_end(it.node_);
        return it_end;
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        ConstIterator it_begin(this->begin());
        return it_begin;
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept {
        ConstIterator it_end(this->end());
        return it_end;
    }

    // Возвращает количество элементов в списке
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список
    [[nodiscard]] bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        if ((size_ == 0) && (head_.next_node == nullptr)) {
            return;
        }
        while (head_.next_node != nullptr) {
            Node* ptr_tmp = head_.next_node->next_node; //сохраняем указатель на второй элемент списка
            delete head_.next_node; //удаляем содержимое первого элемента списка
            head_.next_node = ptr_tmp;
            ptr_tmp = nullptr;
        }
        size_ = 0;
        return;
    }

    // Конструирование односвязного списка на основе initializer_list
    SingleLinkedList(std::initializer_list<Type> values) {
        if (values.size() == 0) {
            SingleLinkedList();
        }
        else {
            // Копируем элементы в буфер tmp
            SingleLinkedList tmp = CopyList(values.begin(), values.end());
            // После того как элементы скопированы, обмениваем данные текущего списка и tmp
            swap(tmp);
        }
    }


    // Конструктор копирования
    SingleLinkedList(const SingleLinkedList& other) {
        // Сначала надо удостовериться, что текущий список пуст
        assert(size_ == 0 && head_.next_node == nullptr);
        
        if (other.IsEmpty()) {
            size_ = 0;
        }
        else {
            // Копируем элементы в буфер tmp
            SingleLinkedList tmp = CopyList(other.begin(), other.end());
            // После того как элементы скопированы, обмениваем данные текущего списка и tmp
            swap(tmp);
            // Теперь tmp пуст, а текущий список содержит копию элементов other
        }
    }

    // Операция присваивания
    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        bool is_equal = (this->head_.next_node == rhs.head_.next_node);
        if (!is_equal) {    
            SingleLinkedList copy_list(rhs);
            swap(copy_list);
        }

        return *this;
    }

    // Обмен содержимого двух списков с использованием метода swap и шаблонной функции swap
    void swap(SingleLinkedList& other) noexcept {
        std::swap(this->head_.next_node, other.head_.next_node);
        std::swap(this->size_, other.size_);
    }


    template<typename _ItTemp>
    SingleLinkedList CopyList(_ItTemp it_begin, _ItTemp it_end) {
        SingleLinkedList copy_list;
        
        Node* prev_node = new Node(*it_begin, nullptr);

        copy_list.head_.next_node = prev_node;
        ++copy_list.size_;
        
        _ItTemp it_start = ++it_begin;
        for (auto it = it_start; it != it_end; it++) {
            // создать новый узел
            Node* ptr = new Node(*it, nullptr);
            prev_node->next_node = ptr;

            prev_node = ptr;
            ptr = nullptr;  // очистка, чтобы память не утекала 

            ++copy_list.size_;
        }
        prev_node = nullptr;
        
        return copy_list;
    }


    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        Iterator it_before_begin(&head_);
        return it_before_begin;
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        const Node* ptr = &head_;
        ConstIterator it_const(const_cast<Node*>(ptr));
        //it_const.node_ = ptr;
        return it_const;
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        const Node* ptr = &head_;
        ConstIterator it_const(const_cast<Node*>(ptr));
        return it_const;
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        // создаем новый узел и в нем ссылаемся на следующий элемент 
        Node* ptr_new_node = new Node(value, pos.node_->next_node);
        
        // с предыдущего узла ссылаемся на новый узел 
        pos.node_->next_node = ptr_new_node;

        // увеличиваем размер
        ++size_;

        Iterator pos_new(ptr_new_node);

        return pos_new;
    }

    void PopFront() noexcept {
        // сохраняем указатель на первый элемент, который надо удалить
        Node* ptr_to_delete = head_.next_node; 
        
        // указатель  фиктивного элемента теперь будет указывать на элемент, следующий за первым
        head_.next_node = ptr_to_delete->next_node;
        
        // удаляем содержимое узла по указателю
        delete ptr_to_delete;
        ptr_to_delete = nullptr;

        // Обновляем размер 
        --size_;

    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        // указатель на удаляемый элемент:
        Node* ptr_to_delete = pos.node_->next_node;

        // ссылаемся из предыдущего узла на новый узел (через один):
        pos.node_->next_node = ptr_to_delete->next_node;

        // удаляем
        delete ptr_to_delete;
        ptr_to_delete = nullptr;

        // возвращаем итератор, следующий за удаленным
        Iterator pos_next(pos.node_->next_node);
        
        // сокращаем данные о размере после удаления 
        --size_;
        
        return pos_next;
    }


private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_ = 0;
};


template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {

    lhs.swap(rhs);
}



template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    } 
    bool is_equal = std::equal(lhs.begin(), lhs.end(), rhs.begin());
    return is_equal;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return true;
    }
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    bool is_left_less_than_right = std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    return is_left_less_than_right;
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs <= rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
} 


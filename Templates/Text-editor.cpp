#include <iostream>
#include <string>
#include <list>
#include <iterator>

using namespace std;

class Editor {
public:
    Editor() {
        it_ = text_.begin();
    }
    // сдвинуть курсор влево
    void Left() {
        if (it_ != text_.begin()) {
            --it_;
        }
    }
    // сдвинуть курсор вправо 
    void Right() {
        if (it_ != text_.end()) {
            ++it_;
        }
    }
    // вставить символ token
    void Insert(char token) {
        text_.insert(it_, token);
    }
    // вырезать не более tokens символов, начиная с текущей позиции курсора
    void Cut(size_t tokens = 1) {
        buffer_.clear();
        if (tokens > (size_t)(distance(it_, text_.end()))) {
            tokens = (size_t)(distance(it_, text_.end()));
        }
        auto end_it = it_;
        advance(end_it, tokens);
        buffer_.assign(it_, end_it);
        it_ = text_.erase(it_, end_it);
    }
    // cкопировать не более tokens символов, начиная с текущей позиции курсора
    void Copy(size_t tokens = 1) {
        buffer_.clear();
        if (tokens > (size_t)(std::distance(it_, text_.end())))
        {
            tokens = (size_t)(std::distance(it_, text_.end()));
        }
        auto endit = it_;
        std::advance(endit, tokens);
        buffer_.assign(it_, endit);
    }
    // вставить содержимое буфера в текущую позицию курсора
    void Paste() {
        text_.insert(it_, buffer_.begin(), buffer_.end());
    }
    // получить текущее содержимое текстового редактора
    string GetText() const {
        return { text_.begin(),text_.end() };
    }

private:
    list<char>::iterator it_;
    list<char> buffer_;
    list<char> text_;

};

int main() {
    Editor editor;
    const string text = "hello, world"s;
    for (char c : text) {
        editor.Insert(c);
    }
    // Текущее состояние редактора: `hello, world|`
    for (size_t i = 0; i < text.size(); ++i) {
        editor.Left();
    }
    // Текущее состояние редактора: `|hello, world`
    editor.Cut(7);
    // Текущее состояние редактора: `|world`
    // в буфере обмена находится текст `hello, `
    for (size_t i = 0; i < 5; ++i) {
        editor.Right();
    }
    // Текущее состояние редактора: `world|`
    editor.Insert(',');
    editor.Insert(' ');
    // Текущее состояние редактора: `world, |`
    editor.Paste();
    // Текущее состояние редактора: `world, hello, |`
    editor.Left();
    editor.Left();
    //Текущее состояние редактора: `world, hello|, `
    editor.Cut(3);  // Будут вырезаны 2 символа
    // Текущее состояние редактора: `world, hello|`
    cout << editor.GetText();
    return 0;
}

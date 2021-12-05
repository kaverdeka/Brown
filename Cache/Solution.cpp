#include "Common.h"

#include <unordered_map>
#include <algorithm>
#include <mutex>

using namespace std;

class LruCache : public ICache {
public:
    LruCache(shared_ptr<IBooksUnpacker> books_unpacker, const Settings& settings) :
        _unpacker(books_unpacker), _settings(settings), _currentRank(0), _totalMemorySize(0) {
        // реализуйте метод
    }

    BookPtr GetBook(const string& book_name) override {
        // реализуйте метод
        // Кэширование производится методом вытеснения давно неиспользуемых элементов (Least Recently Used, LRU).
        // Каждый элемент кэша имеет ранг. При вызове метода GetBook(), если книга с таким названием уже есть в кэше,
        // её ранг поднимается до максимального (строго больше, чем у всех остальных). Если такой книги нет в кэше,
        // то она добавляется в кэш, и её ранг, опять же, выставляется в максимальный. При этом, если общий размер
        // книг превышает ограничение max_memory, из кэша удаляются книги с наименьшим рангом, пока это необходимо.
        // Возможно, пока он полностью не будет опустошён. Если же размер запрошенной книги уже превышает max_memory,
        // то после вызова метода кэш остаётся пустым, то есть книга в него не добавляется.
        //
        // Метод GetBook() может вызываться одновременно из нескольких потоков, поэтому необходимо обеспечить ему безопасность работы в таких условиях.

        lock_guard lock(_mutex);

        Extra* current;
        if(_books.count(book_name) > 0) { // если книга с таким названием уже есть в кэше
            current = &_books[book_name];
            current->rank = _currentRank++;
        } else {
            auto currentBook = _unpacker->UnpackBook(book_name);
            _totalMemorySize += currentBook->GetContent().size();
            while(_totalMemorySize > _settings.max_memory){
                auto it = std::min_element(_books.begin(), _books.end(), [](const CurrentBook& l, const CurrentBook &r){
                    return l.second.rank < r.second.rank;
                });

                if(it == _books.end())
                    break;

                _totalMemorySize -= it->second.book->GetContent().size();
                _books.erase(it);
            }
            if(_books.empty() && _totalMemorySize > _settings.max_memory) {
                _totalMemorySize = 0;
                _currentRank = 0;
                return currentBook;
            }
            current = &_books.insert({book_name, {move(currentBook), _currentRank++}}).first->second;
        }
        return current->book;
    }

private:
    struct Extra {
        BookPtr book;
        int rank;
    };

    std::shared_ptr<IBooksUnpacker> _unpacker;
    int _currentRank;
    int _totalMemorySize;

    using CurrentBook = std::pair<string, Extra>;
    std::unordered_map<string, Extra> _books;
    Settings _settings;
    std::mutex _mutex;
};


unique_ptr<ICache> MakeCache(
        shared_ptr<IBooksUnpacker> books_unpacker,
        const ICache::Settings& settings
) {
    // реализуйте функцию
    return make_unique<LruCache>(books_unpacker, settings);
}


enum class Size {
    tiny,
    medium,
    big,
};

enum class Window {
    fullscreen,
    windowed,
};

struct Hints {
    int size_hints {};
    int window_hints {};
};

class Widget {
public:
    Widget(int x) : m_n { x } {}

private:
    int m_n {};
};

template <Size S, Window W> class basic_window {
public:
    basic_window() { Widget some_widget { m_hints.size_hints ^ m_hints.window_hints }; }

private:
    static constexpr Hints m_hints = [] consteval {
        Hints h {};
        if constexpr (S == Size::tiny) {
            h.size_hints = 1;
        }
        if constexpr (W == Window::fullscreen) {
            h.window_hints = 2;
        }
        return h;
    }();
};

using my_window = basic_window<Size::tiny, Window::fullscreen>;

int main() {
    my_window special_window{};

    return 0;
}

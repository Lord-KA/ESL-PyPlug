namespace fuck {

char *hello(int a) {
    return "hello world!";
}
class image {
public:
    int getPixel(int x, int y) {
        //std::cerr << "DEBUG: called getPixel with args: x = " << x << "; y = " << y << std::endl;
        return x + y;
    }
private:
    int n;
};
}

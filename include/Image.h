#pragma once


namespace CoreEx
{

    template<typename T>
    class Image
    {

    private:
        T* _data;
        int _width;
        int _height;

    public:
        Image<T>(int width, int height);
        Image<T>(const Image& src_image);
        Image<T>& operator= (const Image<T>& src_image) = delete;
        ~Image();

        T* operator[](size_t idx_y);


        size_t GetSize() const;
        void Clear();

        void Resize(int width, int height);







    };

    template class Image<float>;

}
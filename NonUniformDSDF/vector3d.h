#ifndef VECTOR3D_H
#define VECTOR3D_H

template<typename T>
class vector3d {
private:
    size_t d1, d2, d3;
    std::vector<T> data;

public:
    int dim1() const { return d1; }
    int dim2() const { return d2; }
    int dim3() const { return d3; }

    const std::vector<T> innerVector() { return data; }

    vector3d(size_t d1, size_t d2, size_t d3, T const& t = T()) :
        d1(d1), d2(d2), d3(d3), data(d1 * d2 * d3, t)
    {}

    vector3d(size_t d, T const& t = T()) :
        vector3d(d, d, d, t)
    {}

    T& operator()(size_t i, size_t j, size_t k) {
        return data[i * d2 * d3 + j * d3 + k];
    }

    T const& operator()(size_t i, size_t j, size_t k) const {
        return data[i * d2 * d3 + j * d3 + k];
    }

    T& get(size_t i, size_t j, size_t k) {
        return data[i * d2 * d3 + j * d3 + k];
    }

    T const& get(size_t i, size_t j, size_t k) const {
        return data[i * d2 * d3 + j * d3 + k];
    }
};
#endif
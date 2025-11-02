#include "../../include/phy/filter.h"


std::vector<complex_d> upsampling(const std::vector<complex_d>& symbols, int count_smpl_symbol)
{
    std::vector<complex_d> upsampled(symbols.size() * count_smpl_symbol, complex_d(0.0, 0.0));

    for (size_t i = 0; i < symbols.size(); ++i) {
        upsampled[i * count_smpl_symbol] = symbols[i];
    }

    return upsampled;
}

std::vector<complex_d> rect_pulse(int count_smpl_symbol)
{
    std::vector<complex_d> pulse(count_smpl_symbol);
    for(int i = 0; i < count_smpl_symbol; i++){
        pulse[i] = complex_d(1,0);
    }

    return pulse;
}

std::vector <complex_d> convolve(const std::vector <complex_d> &symb, const std::vector <complex_d> &pulse)
{
    int size_symb = symb.size();
    int pulse_size = pulse.size();
    std::vector<complex_d> padded_symb(pulse_size - 1, complex_d(0.0, 0.0));
    padded_symb.insert(padded_symb.end(), symb.begin(), symb.end());

    std::vector <complex_d> y(size_symb + pulse_size - 1, complex_d(0,0));

    for (int i = 0; i < int(padded_symb.size() - (pulse_size-1)); i++){
        for (int j = 0; j < pulse_size; j++){
            y[i] += padded_symb[i+j] * pulse[j];
        } 
    }

    return y;
}

std::vector<complex_d> convolve2(const std::vector<complex_d>& x,
                                const std::vector<complex_d>& h)
{
    size_t n = x.size();
    size_t m = h.size();
    std::vector<complex_d> y(n, complex_d(0.0, 0.0));

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            y[i + j] += x[i] * h[j];
        }
    }

    return y;
}
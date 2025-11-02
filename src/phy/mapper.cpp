#include "../../include/phy/mapper.h"

std::vector<complex_d> bpsk(const std::vector <int8_t> &bits)
{
    std::vector<complex_d> bpsk_symb(bits.size(), complex_d(0,0));

    for (int i = 0; i < bits.size(); i++){
        bpsk_symb[i].real(bits[i] * 2 - 1);
    }

    return bpsk_symb;
}

std::vector<complex_d> qpsk(const std::vector<int8_t> &bits)
{
    size_t n_sym = bits.size() / 2;
    std::vector<complex_d> qpsk_symb(n_sym);

    const double norm = 1.0 / std::sqrt(2.0);

    for (size_t i = 0; i < n_sym; i++) {
        int b0 = bits[2 * i];     
        int b1 = bits[2 * i + 1]; 

        double i_part = (b0 ? 1.0 : -1.0);
        double q_part = (b1 ? 1.0 : -1.0);

        qpsk_symb[i] = complex_d(i_part, q_part) * norm;
    }

    return qpsk_symb;
}


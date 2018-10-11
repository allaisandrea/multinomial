#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>

template <uint64_t k> using Configuration = std::array<uint32_t, k>;

template <uint64_t k>
std::ostream &operator<<(std::ostream &strm, const Configuration<k> &x) {
  strm << "[";
  for (uint64_t i : x) {
    strm << std::setw(3) << i;
  }
  strm << "]";
  return strm;
}

template <uint64_t k> class ConfigurationIterator {
public:
  ConfigurationIterator(const Configuration<k> &x) : _x(x){};
  void operator++() {
    uint64_t i = k - 2;
    ++_x.at(i);
    while (i > 0 && _x.at(i) > _x.at(k - 1)) {
      --i;
      ++_x.at(i);
    }
    while (i + 2 < k) {
      _x.at(i + 1) = _x.at(i);
      ++i;
    }
  }

  bool operator!=(const ConfigurationIterator &other) { return _x != other._x; }

  const Configuration<k> &operator*() { return _x; }

private:
  Configuration<k> _x;
};

template <uint64_t k> class Configurations {
public:
  Configurations(uint64_t n) : _n(n) {}
  ConfigurationIterator<k> begin() {
    Configuration<k> x;
    x.fill(0);
    x[k - 1] = _n;
    return ConfigurationIterator<k>(x);
  }

  ConfigurationIterator<k> end() {
    Configuration<k> x;
    x.fill(_n + 1);
    x[k - 1] = _n;
    return ConfigurationIterator<k>(x);
  }

private:
  uint64_t _n;
};

template <uint64_t k> class MultinomialTable {
public:
  MultinomialTable(uint64_t nMax) : _nMax(nMax), _table(std::pow(nMax, k), 0) {
    _table[0] = 1;
    for (uint64_t n = 1; n < nMax; ++n) {
      for (const Configuration<k> &x : Configurations<k>(n - 1)) {
        const uint64_t countX = at(x);
        Configuration<k> y = x;
        for (uint64_t i = k - 1; i < k; --i) {
          ++y[i];
          at(y) += countX;
        }
      };
    }
  }

  uint64_t &at(const Configuration<k> &x) { return _table.at(FlatIndex(x)); }

  uint64_t at(const Configuration<k> &x) const {
    return _table.at(FlatIndex(x));
  }

  uint64_t nMax() const { return _nMax; }

private:
  uint64_t FlatIndex(const Configuration<k> &x) const {
    uint64_t i = 0;
    for (uint64_t a : x) {
      i = _nMax * i + a;
    }
    return i;
  }

  const uint64_t _nMax;
  std::vector<uint64_t> _table;
};

template <uint64_t k> uint64_t Maximum(const Configuration<k> &x) {
  uint64_t max = x[0];
  for (uint64_t i = 1; i < x.size(); ++i) {
    max = std::max<uint64_t>(x[i] - x[i - 1], max);
  }
  return max;
}

template <uint64_t k> uint64_t SumOfSquares(const Configuration<k> &x) {
  uint64_t sumOfSquares = std::pow(x[0], 2);
  for (uint64_t i = 1; i < x.size(); ++i) {
    sumOfSquares += std::pow(x[i] - x[i - 1], 2);
  }
  return sumOfSquares;
}

template <uint64_t k> void PrintCounts(const MultinomialTable<k> &table) {
  for (uint32_t n = 0; n < table.nMax(); ++n) {
    std::cout << "=== n: " << n << "  ===" << std::endl;
    uint64_t total = 0;
    for (const Configuration<k> &x : Configurations<k>(n)) {
      const uint64_t countX = table.at(x);
      total += countX;
      std::cout << x << ": " << countX << std::endl;
    }
    std::cout << "total: " << total << std::endl;
  }
}

template <uint64_t k> void PrintStats(const MultinomialTable<k> &table) {
  const auto flags = std::cout.flags();
  std::cout << std::fixed << std::setprecision(5);
  for (uint32_t n = 0; n < table.nMax(); ++n) {
    std::cout << "=== n: " << n << "  ===" << std::endl;
    std::vector<uint64_t> frequencyOfMaximum(n + 1, 0);
    std::vector<uint64_t> sumOfSquaresFrequency(std::pow(n, 2) + 1, 0);
    for (const Configuration<k> &x : Configurations<k>(n)) {
      frequencyOfMaximum.at(Maximum(x)) += table.at(x);
      sumOfSquaresFrequency.at(SumOfSquares(x)) += table.at(x);
    }

    std::cout << "Frequency of maximum" << std::endl;
    for (size_t i = 0; i < frequencyOfMaximum.size(); ++i) {
      if (frequencyOfMaximum[i] != 0) {
        std::cout << std::setw(5) << i << std::setw(12)
                  << float(frequencyOfMaximum[i]) / std::pow(k, n) << std::endl;
      }
    }

    std::cout << "Frequency of sum of squares" << std::endl;
    for (size_t i = 0; i < sumOfSquaresFrequency.size(); ++i) {
      if (sumOfSquaresFrequency[i] != 0) {
        std::cout << std::setw(5) << i << std::setw(10)
                  << (i - n * n / float(k)) / float(k) << std::setw(12)
                  << float(sumOfSquaresFrequency[i]) / std::pow(k, n)
                  << std::endl;
      }
    }
  }
  std::cout.flags(flags);
}

int main(int argc, char **argv) {
  const MultinomialTable<6> table(25);
  PrintStats(table);
  return 0;
}

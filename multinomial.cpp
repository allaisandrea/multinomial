#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using Configuration = std::vector<uint32_t>;

std::ostream &operator<<(std::ostream &strm, const Configuration &x) {
  strm << "[";
  for (uint64_t i : x) {
    strm << std::setw(3) << i;
  }
  strm << "]";
  return strm;
}

class ConfigurationIterator {
public:
  void operator++() {
    const uint64_t k = _x.size();
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

  const Configuration &operator*() const { return _x; }

  Configuration &operator*() { return _x; }

private:
  Configuration _x;
};

class Configurations {
public:
  Configurations(uint64_t k, uint64_t n) : _k(k), _n(n) {}
  ConfigurationIterator begin() {
    ConfigurationIterator result;
    (*result).assign(_k, 0);
    (*result).at(_k - 1) = _n;
    return result;
  }

  ConfigurationIterator end() {
    ConfigurationIterator result;
    (*result).assign(_k, _n + 1);
    (*result).at(_k - 1) = _n;
    return result;
  }

private:
  uint64_t _k;
  uint64_t _n;
};

class MultinomialTable {
public:
  MultinomialTable(uint64_t k, uint64_t nMax)
      : _k(k), _nMax(nMax), _table(std::pow(nMax, k), 0) {
    _table[0] = 1;
    for (uint64_t n = 1; n < nMax; ++n) {
      for (const Configuration &x : Configurations(k, n - 1)) {
        const uint64_t countX = at(x);
        Configuration y = x;
        for (uint64_t i = k - 1; i < k; --i) {
          ++y[i];
          at(y) += countX;
        }
      };
    }
  }

  uint64_t &at(const Configuration &x) { return _table.at(FlatIndex(x)); }

  uint64_t at(const Configuration &x) const { return _table.at(FlatIndex(x)); }

  uint64_t nMax() const { return _nMax; }
  uint64_t k() const { return _k; }

private:
  uint64_t FlatIndex(const Configuration &x) const {
    uint64_t i = 0;
    for (uint64_t a : x) {
      i = _nMax * i + a;
    }
    return i;
  }

  const uint64_t _k;
  const uint64_t _nMax;
  std::vector<uint64_t> _table;
};

uint64_t Maximum(const Configuration &x) {
  uint64_t max = x[0];
  for (uint64_t i = 1; i < x.size(); ++i) {
    max = std::max<uint64_t>(x[i] - x[i - 1], max);
  }
  return max;
}

uint64_t SumOfSquares(const Configuration &x) {
  uint64_t sumOfSquares = std::pow(x[0], 2);
  for (uint64_t i = 1; i < x.size(); ++i) {
    sumOfSquares += std::pow(x[i] - x[i - 1], 2);
  }
  return sumOfSquares;
}

void PrintSumOfSquares(const MultinomialTable &table) {
  const auto flags = std::cout.flags();
  std::cout << "k,n,sumOfSquares,count" << std::endl;
  std::vector<uint64_t> sumOfSquaresFrequency;
  for (uint32_t n = 0; n < table.nMax(); ++n) {
    sumOfSquaresFrequency.assign(std::pow(n, 2) + 1, 0);
    for (const Configuration &x : Configurations(table.k(), n)) {
      sumOfSquaresFrequency.at(SumOfSquares(x)) += table.at(x);
    }
    for (size_t i = 0; i < sumOfSquaresFrequency.size(); ++i) {
      if (sumOfSquaresFrequency[i] != 0) {
        std::cout << table.k() << "," << n << "," << i << ","
                  << sumOfSquaresFrequency[i] << std::endl;
      }
    }
  }
  std::cout.flags(flags);
}

int main(int argc, char **argv) {
  const MultinomialTable table(6, 25);
  PrintSumOfSquares(table);
  return 0;
}

struct NumberTheory {
  NumberTheory(int n) : is_prime(n, true), mu(n), phi(n) {
    mu[1] = phi[1] = 1;
    for (int i = 2; i < n; ++i) {
      if (is_prime[i]) {
        prime.push_back(i);
        mu[i] = -1;
        phi[i] = i - 1;
      }
      for (int p : prime) {
        int k = i * p;
        if (k >= n) break;
        is_prime[k] = false;
        dbg(k, i, p, phi[i]);
        if (i % p) {
          mu[k] = -mu[i];
          phi[k] = phi[i] * (p - 1);
        } else {
          phi[k] = phi[i] * p;
          break;
        }
      }
    }
  }

  std::vector<int> mu, phi;
  std::vector<bool> is_prime;
  std::vector<int> prime;
};

/*
 * ssk_kernel.c
 *
 *  Created on: 2018/06/18
 *      Author: mitsuiyosuke
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define IDX(o, p, q, width, height) ((o) * ((width) * (height)) + (p) * (height) + (q))

static double _computeK(char *s, char *t, int k, float l, double *K_prime) {
	int M = (int)strlen(s);
	int N = (int)strlen(t);
	double K_val = 0.;
	int m;
	double tot;
	int n_t = strlen(t);
	int n_s = strlen(s);
	if (strlen(t) < k)
		return 0.;

	for (m=k; m < n_s + 1; m++) {
		tot = 0.;
		for(int j=0; j < n_t; j++) {
			if (s[m - 1] == t[j]) {
				tot += K_prime[IDX(k - 1, m - 1, j, (M + 1), (N + 1))];
			}
		}
		K_val += l * l * tot;
	}
	return K_val;
}

static double *_computeKPrime(char *s_, char *t_, int k, double l) {
	int M = strlen(s_);
	int N = strlen(t_);
	double *K_prime = malloc(sizeof(double) * k * (M + 1) * (N + 1));
	for (int i=0; i < k; i++) {
		for (int m=0; m < (M + 1) ; m++) {
			for (int n=0; n < (N + 1) ; n++)
				K_prime[IDX(i, m, n, (M + 1), (N + 1))] = 1.;
		}
	}

	// not necessary ?
	double *K_dprime = calloc(1, sizeof(double) * k * (M + 1) * (N + 1));

	for (int i=1; i < k; i++) {
		for (int m=0; m < (M + 1) ; m++) {
			for (int n=0; n < (N + 1) ; n++) {
				int min_idx = (m < n) ? m : n;
				if (min_idx < i) {
					K_prime[IDX(i, m, n, (M + 1), (N + 1))] = 0.;
					continue;
				}
				if (s_[m - 1] != t_[n - 1]) {
					K_dprime[IDX(i, m, n, (M + 1), (N + 1))] = l*K_dprime[IDX(i, m, n-1, (M + 1), (N + 1))];
				}else{
					K_dprime[IDX(i, m, n, (M + 1), (N + 1))] = l*(K_dprime[IDX(i, m, n-1, (M + 1), (N + 1))] + l*K_prime[IDX(i-1, m-1, n-1, (M + 1), (N + 1))]);
				}
				K_prime[IDX(i, m, n, (M + 1), (N + 1))] = l*K_prime[IDX(i, m-1, n, (M + 1), (N + 1))] + K_dprime[IDX(i, m, n, (M + 1), (N + 1))];
			}
		}
	}
	free(K_dprime);
	return K_prime;
}

double sskKernel(char *s, char *t, int k, double l) {
	if (strcmp(s, t) == 0) return 1.0;
	else if (strlen(s) < k || strlen(t) < k) return 0.0;

	double *K_prime = _computeKPrime(s, t, k, l);
	double K_st = _computeK(s, t, k, l, K_prime);
	free(K_prime);

	K_prime = _computeKPrime(s, s, k, l);
	double K_ss = _computeK(s, s, k, l, K_prime);
	free(K_prime);

	K_prime = _computeKPrime(t, t, k, l);
	double K_tt = _computeK(t, t, k, l, K_prime);
	free(K_prime);

	double denominator = (K_ss * K_tt != 0.) ? sqrt(K_ss * K_tt) : 10e-30;
	return K_st / denominator;
}

int main(void) {
	printf("sskKernel:%.5f\n", sskKernel("ac", "abc", 2, 0.5));
}

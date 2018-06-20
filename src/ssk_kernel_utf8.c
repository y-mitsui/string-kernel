/*
 * ssk_kernel_utf8.c
 *
 *  Created on: 2018/06/18
 *      Author: mitsuiyosuke
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define IDX(o, p, q, width, height) ((o) * ((width) * (height)) + (p) * (height) + (q))
typedef struct  {
	char *seq[1024];
	int sizes[1024];
	int seq_len;
}UTF8Seq;

static int eachChar(char *str, char **charactors, int *char_sizes, int limit){
	unsigned char lead;
	int char_size = 0, n_charactors=0;
	int n_iter = strlen(str);
	if (n_iter > limit) n_iter = limit;
	for(int i=0; i < n_iter; i+=char_size) {
		lead = str[i];
		if (lead < 0x80) {
			char_size = 1;
		} else if (lead < 0xE0) {
			char_size = 2;
		} else if (lead < 0xF0) {
			char_size = 3;
		} else {
			char_size = 4;
		}
		char_sizes[n_charactors] = char_size;
		charactors[n_charactors++] = &str[i];
	}
	return n_charactors;
}

static double _computeK(UTF8Seq *s, UTF8Seq *t, int k, float l, double *K_prime) {
	int M = s->seq_len;
	int N = t->seq_len;
	double K_val = 0.;
	int m;
	double tot;
	if (N < k)
		return 0.;

	for (m=k; m < (M + 1); m++) {
		tot = 0.;
		for(int j=0; j < N; j++) {
			if (s->sizes[m - 1] == t->sizes[j]
			&&  memcmp(s->seq[m - 1], t->seq[j], s->sizes[m - 1]) == 0) {
				tot += K_prime[IDX(k - 1, m - 1, j, (M + 1), (N + 1))];
			}
		}
		K_val += l * l * tot;
	}
	return K_val;
}

static double *_computeKPrime(UTF8Seq *s_, UTF8Seq *t_, int k, double l) {
	int M = s_->seq_len;
	int N = t_->seq_len;
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
				if (s_->sizes[m - 1] != t_->sizes[n - 1]
				||  memcmp(s_->seq[m - 1], t_->seq[n - 1], s_->sizes[m - 1]) != 0) {
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

double sskKernelUtf8(char *s, char *t, int k, double l) {
	UTF8Seq s_seq, t_seq;
	s_seq.seq_len = eachChar(s, s_seq.seq, s_seq.sizes, 1024);
	t_seq.seq_len = eachChar(t, t_seq.seq, t_seq.sizes, 1024);

	if (strcmp(s, t) == 0) return 1.0;
	else if (s_seq.seq_len < k || t_seq.seq_len < k) return 0.0;

	double *K_prime = _computeKPrime(&s_seq, &t_seq, k, l);
	double K_st = _computeK(&s_seq, &t_seq, k, l, K_prime);
	free(K_prime);

	K_prime = _computeKPrime(&s_seq, &s_seq, k, l);
	double K_ss = _computeK(&s_seq, &s_seq, k, l, K_prime);
	free(K_prime);

	K_prime = _computeKPrime(&t_seq, &t_seq, k, l);
	double K_tt = _computeK(&t_seq, &t_seq, k, l, K_prime);
	free(K_prime);

	double denominator = (K_ss * K_tt != 0.) ? sqrt(K_ss * K_tt) : 10e-30;
	return K_st / denominator;
}

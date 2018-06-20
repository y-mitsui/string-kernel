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

double sskKernelUtf8Multi(char **s_arr, char **t_arr, int n_s, int n_t, int k, double l) {
	UTF8Seq s_seq, t_seq, s_seq2, t_seq2;
	double *K_prime;
	double kernel_tot = 0.;
	double norm_s_tot = 0.;
	double norm_t_tot = 0.;
	for (int i=0; i < n_s; i++) {
		s_seq.seq_len = eachChar(s_arr[i], s_seq.seq, s_seq.sizes, 1024);
		for (int j=0; j < n_t; j++) {
			t_seq.seq_len = eachChar(t_arr[j], t_seq.seq, t_seq.sizes, 1024);

			K_prime = _computeKPrime(&s_seq, &t_seq, k, l);
			double K_st = _computeK(&s_seq, &t_seq, k, l, K_prime);
			free(K_prime);
			kernel_tot += K_st;
		}
	}

	for (int i=0; i < n_s; i++) {
		s_seq.seq_len = eachChar(s_arr[i], s_seq.seq, s_seq.sizes, 1024);
		for (int j=0; j < n_s; j++) {
			s_seq2.seq_len = eachChar(s_arr[j], s_seq2.seq, s_seq2.sizes, 1024);
			K_prime = _computeKPrime(&s_seq, &s_seq2, k, l);
			double K_ss = _computeK(&s_seq, &s_seq2, k, l, K_prime);
			free(K_prime);
			norm_s_tot += K_ss;
		}
	}

	for (int i=0; i < n_t; i++) {
		t_seq.seq_len = eachChar(t_arr[i], t_seq.seq, t_seq.sizes, 1024);
		for (int j=0; j < n_t; j++) {
			t_seq2.seq_len = eachChar(t_arr[j], t_seq2.seq, t_seq2.sizes, 1024);
			K_prime = _computeKPrime(&t_seq2, &t_seq, k, l);
			double K_tt = _computeK(&t_seq2, &t_seq, k, l, K_prime);
			free(K_prime);
			norm_t_tot += K_tt;
		}
	}
	double denominator = (norm_s_tot * norm_t_tot != 0.) ? sqrt(norm_s_tot * norm_t_tot) : 10e-30;
	return kernel_tot / denominator;
}

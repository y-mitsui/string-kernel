/*
 * ssk_kernel.h
 *
 *  Created on: 2018/06/18
 *      Author: mitsuiyosuke
 */

#ifndef SRC_SSK_KERNEL_H_
#define SRC_SSK_KERNEL_H_

double sskKernel(char *s, char *t, int k, double l);
double sskKernelUtf8(char *s, char *t, int k, double l);
double sskKernelUtf8Multi(char **s_arr, char **t_arr, int n_s, int n_t, int k, double l);
#endif /* SRC_SSK_KERNEL_H_ */

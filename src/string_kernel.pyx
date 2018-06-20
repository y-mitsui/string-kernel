from libc.stdlib cimport malloc, free
from libc.string cimport strdup

cdef extern from "ssk_kernel.h":
    double sskKernel(char *s, char *t, int k, double l)
    double sskKernelUtf8(char *s, char *t, int k, double l)
    double sskKernelUtf8Multi(char **s_arr, char **t_arr, int n_s, int n_t, int k, double l)
    
def string_kernel(s, t, k, l):
    return sskKernel(s, t, k, l)

def stringKernelUTF8(s, t, k, l):
    return sskKernelUtf8(s, t, k, l)

def stringKernelUTF8Multi(s_seq, t_seq, k, l):
    cdef char **p_s = <char**>malloc(sizeof(char*) * (len(s_seq) + 1))
    cdef char **t_s = <char**>malloc(sizeof(char*) * (len(t_seq) + 1))
    for i in range(len(s_seq)):
        p_s[i] = strdup(s_seq[i])
    for i in range(len(t_seq)):
        t_s[i] = strdup(t_seq[i])
        
    r = sskKernelUtf8Multi(p_s, t_s, len(s_seq),len(t_seq),  k, l)
    
    for i in range(len(s_seq)):
        free(p_s[i])
    for i in range(len(t_seq)):
        free(t_s[i])
    free(p_s)
    free(t_s)
    return r;
    
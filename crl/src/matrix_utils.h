/*
 * File name: matrix_utils.h
 * Date:      2011/08/11 11:33
 * Author:    Jan Faigl
 */

#ifndef __MATRIX_UTILS_H__
#define __MATRIX_UTILS_H__

#include <crl/assert.h>

namespace crl {

template <typename T> struct SMatrix {

   SMatrix(const int nrows, const int ncols)
       : SIZE(nrows * ncols), NROWS(nrows), NCOLS(ncols)
   {
      _values = new T[SIZE];
   }

   SMatrix(const SMatrix &a)
       : SIZE(a.NROWS * a.NCOLS), NROWS(a.NROWS), NCOLS(a.NCOLS)
   {
      _values = new T[SIZE];
      for (int i = 0; i < SIZE; ++i) {
         _values[i] = a._values[i];
      }
   }

   ~SMatrix() { delete[] _values; }

   inline T *values(void) { return &_values[0]; }
   inline const T *values(void) const { return &_values[0]; }

   inline int index(int r, int c) const { return r * NCOLS + c; }
   inline int row(int idx) const { return idx / NCOLS; }
   inline int col(int idx) const { return idx % NCOLS; }

   inline T &operator[](int i) { return _values[i]; }
   inline const T &operator[](int i) const { return _values[i]; }
   inline T &operator()(int i) { return _values[i]; }
   inline const T &operator()(int i) const { return _values[i]; }

   inline T &at(int r, int c) { return _values[r * NCOLS + c]; }
   inline const T &at(int r, int c) const { return _values[r * NCOLS + c]; }
   inline T &operator()(int r, int c) { return _values[r * NCOLS + c]; }
   inline const T &operator()(int r, int c) const
   {
      return _values[r * NCOLS + c];
   }

   inline void reset(T v)
   {
      for (int i = 0; i < SIZE; ++i) {
         _values[i] = v;
      }
   }

   T *_values;
   const int SIZE;
   const int NROWS;
   const int NCOLS;
};

typedef SMatrix<double> DoubleMatrix;
typedef SMatrix<float> FloatMatrix;
typedef SMatrix<int> IntMatrix;
typedef SMatrix<unsigned char> ByteMatrix;

} // end namespace crl

#endif

/* end of matrix_utils.h */

/*
 * Enlightement Hidden Ninjas.
 *
 * Copyright 2012 Hermet (ChunEon Park)
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */


#ifndef __MATH_H__
#define __MATH_H__

#define TOLERANCE 0.000001

template<typename T>
struct Vector2
{
   T x;
   T y;
   Vector2<T>(): x(0), y(0) {}
   Vector2<T>(T x, T y): x(x), y(y) {}
   Vector2<T>(const Vector2<T>& v): x(v.x), y(v.y) {}

   const Vector2<T>& operator-=(const Vector2<T>& rhs)
   {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
   }

   const Vector2<T>& operator+=(const Vector2<T>& rhs)
   {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
   }

   const Vector2<T>& operator-(const Vector2<T>& rhs)
   {
        return Vector2<T>(*this) -= rhs;
   }

   const Vector2<T>& operator+(const Vector2<T>& rhs)
   {
        return Vector2<T>(*this) += rhs;
   }

   const Vector2<T>& operator*=(const T& rhs)
   {
        this->x *= rhs;
        this->y *= rhs;
        printf("FIXME: What the Hell??? %f\n", rhs);

        return *this;
   }

   const Vector2<T>& operator*(const T rhs) const
   {
        return Vector2<T>(*this) *= rhs;
   }

   void Normalize()
   {
        T length = sqrt((this->x * this->x) + (this->y * this->y));

        if (length >= TOLERANCE)
          {
             this->x /= length;
             this->y /= length;
          }
        else
          {
             this->x = this->y = 0;
          }
   }

};

#endif


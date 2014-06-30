/* Elapsed time types - for easy-to-use measurements of elapsed time
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2011 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef ELAPSED_TIME_H
#define ELAPSED_TIME_H

class ElapsedMillis
{
private:
	unsigned long ms;
public:
	ElapsedMillis(void) { ms = millis(); }
	ElapsedMillis(unsigned long val) { ms = millis() - val; }
	ElapsedMillis(const ElapsedMillis &orig) { ms = orig.ms; }
	operator unsigned long () const { return millis() - ms; }
	ElapsedMillis & operator = (const ElapsedMillis &rhs) { ms = rhs.ms; return *this; }
	ElapsedMillis & operator = (unsigned long val) { ms = millis() - val; return *this; }
	ElapsedMillis & operator -= (unsigned long val)      { ms += val ; return *this; }
	ElapsedMillis & operator += (unsigned long val)      { ms -= val ; return *this; }
	ElapsedMillis operator - (int val) const           { ElapsedMillis r(*this); r.ms += val; return r; }
	ElapsedMillis operator - (unsigned int val) const  { ElapsedMillis r(*this); r.ms += val; return r; }
	ElapsedMillis operator - (long val) const          { ElapsedMillis r(*this); r.ms += val; return r; }
	ElapsedMillis operator - (unsigned long val) const { ElapsedMillis r(*this); r.ms += val; return r; }
	ElapsedMillis operator + (int val) const           { ElapsedMillis r(*this); r.ms -= val; return r; }
	ElapsedMillis operator + (unsigned int val) const  { ElapsedMillis r(*this); r.ms -= val; return r; }
	ElapsedMillis operator + (long val) const          { ElapsedMillis r(*this); r.ms -= val; return r; }
	ElapsedMillis operator + (unsigned long val) const { ElapsedMillis r(*this); r.ms -= val; return r; }
};

class ElapsedMicros
{
private:
	unsigned long us;
public:
	ElapsedMicros(void) { us = micros(); }
	ElapsedMicros(unsigned long val) { us = micros() - val; }
	ElapsedMicros(const ElapsedMicros &orig) { us = orig.us; }
	operator unsigned long () const { return micros() - us; }
	ElapsedMicros & operator = (const ElapsedMicros &rhs) { us = rhs.us; return *this; }
	ElapsedMicros & operator = (unsigned long val) { us = micros() - val; return *this; }
	ElapsedMicros & operator -= (unsigned long val)      { us += val ; return *this; }
	ElapsedMicros & operator += (unsigned long val)      { us -= val ; return *this; }
	ElapsedMicros operator - (int val) const           { ElapsedMicros r(*this); r.us += val; return r; }
	ElapsedMicros operator - (unsigned int val) const  { ElapsedMicros r(*this); r.us += val; return r; }
	ElapsedMicros operator - (long val) const          { ElapsedMicros r(*this); r.us += val; return r; }
	ElapsedMicros operator - (unsigned long val) const { ElapsedMicros r(*this); r.us += val; return r; }
	ElapsedMicros operator + (int val) const           { ElapsedMicros r(*this); r.us -= val; return r; }
	ElapsedMicros operator + (unsigned int val) const  { ElapsedMicros r(*this); r.us -= val; return r; }
	ElapsedMicros operator + (long val) const          { ElapsedMicros r(*this); r.us -= val; return r; }
	ElapsedMicros operator + (unsigned long val) const { ElapsedMicros r(*this); r.us -= val; return r; }
};

#endif
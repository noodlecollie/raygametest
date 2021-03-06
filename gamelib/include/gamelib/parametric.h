#pragma once

// These functions map an input [0 1] to an output [0 1].
// If the input is out of range, it is clamped.

// y = t
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4IiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwfV0-
float Parametric_LinearRampUp(float t);

// y = -t + 1
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoLXgpKzEiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDB9XQ--
float Parametric_LinearRampDown(float t);

// y = (sin((2 * pi * t) - (pi / 2)) + 1) / 2
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoc2luKCgyKnBpKngpLShwaS8yKSkrMSkvMiIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMH1d
float Parametric_SinePeak(float t);

// y = (sin((2 * pi * t) + (pi / 2)) + 1) / 2
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoc2luKCgyKnBpKngpKyhwaS8yKSkrMSkvMiIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMH1d
float Parametric_SineTrough(float t);

// y = (sin((pi * t) - (pi / 2)) + 1) / 2
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoc2luKChwaSp4KS0ocGkvMikpKzEpLzIiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDB9XQ--
float Parametric_HalfSineUp(float t);

// y = (sin((pi * t) + (pi / 2)) + 1) / 2
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoc2luKChwaSp4KSsocGkvMikpKzEpLzIiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDB9XQ--
float Parametric_HalfSineDown(float t);

// y = t^2
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4XjIiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDB9XQ--
float Parametric_ParabolaEaseInUp(float t);

// y = -((t - 1)^2) + 1
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiItKCh4LTEpXjIpKzEiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDB9XQ--
float Parametric_ParabolaEaseOutUp(float t);

// y = -(t^2) + 1
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiItKHheMikrMSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMH1d
float Parametric_ParabolaEaseInDown(float t);

// y = (t - 1)^2
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoeC0xKV4yIiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwfV0-
float Parametric_ParabolaEaseOutDown(float t);

// y = t^3
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiJ4XjMiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjEwMDB9XQ--
float Parametric_CubicEaseInUp(float t);

// y = ((t - 1)^3) + 1
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoKHgtMSleMykrMSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMH1d
float Parametric_CubicEaseOutUp(float t);

// y = -(t^3) + 1
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiItKHheMykrMSIsImNvbG9yIjoiIzAwMDAwMCJ9LHsidHlwZSI6MTAwMH1d
float Parametric_CubicEaseInDown(float t);

// y = -((t - 1)^3)
// http://www.fooplot.com/#W3sidHlwZSI6MCwiZXEiOiItKCh4LTEpXjMpIiwiY29sb3IiOiIjMDAwMDAwIn0seyJ0eXBlIjoxMDAwfV0-
float Parametric_CubicEaseOutDown(float t);

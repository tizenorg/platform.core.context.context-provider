/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "visit_categer.h"
#include "mahal.h"
#include <time.h>
#include "../place_recognition_types.h"
#include "prob_features_model.h"
#include <Types.h>

// categorizer model parameters trained offline (implemented in python):
const std::map<int, ctx::MahalModel> ctx::VisitCateger::__models({
{
	PLACE_CATEG_ID_HOME,
	ctx::MahalModel(
	{
		0.588408417316, 0.475154840361, -0.564085141865, 0.077711893790, 0.782630398597, -0.650926403250, -0.101943950378
	},
	{
		0.797977863370, -0.133179879257, 0.242803062646, -0.059448581046, -0.117039646748, 0.097645535057, 0.014250290052,
		-0.133179879257, 1.028900241400, -0.252245407243, 0.058455883835, -0.283950879851, 0.172204076583, 0.247906065767,
		0.242803062646, -0.252245407243, 1.832134785177, 0.327188225606, 0.874905851092, -0.371088938012, -1.289816938938,
		-0.059448581046, 0.058455883835, 0.327188225606, 1.455090164348, 0.138363721074, 0.216985279422, -1.113021128017,
		-0.117039646748, -0.283950879851, 0.874905851092, 0.138363721074, 1.379674755873, -0.977922749615, -0.738895486376,
		0.097645535057, 0.172204076583, -0.371088938012, 0.216985279422, -0.977922749615, 0.899928922718, -0.158101631251,
		0.014250290052, 0.247906065767, -1.289816938938, -1.113021128017, -0.738895486376, -0.158101631251, 2.644105309746
	})
},
{
	PLACE_CATEG_ID_WORK,
	ctx::MahalModel(
	{
		-0.128092670982, -0.762177819157, 0.262924477521, -0.412038966097, -1.049141893517, 1.104760800499, -0.628939955525
	},
	{
		15.751249839350, 11.389025401325, -9.885346240379, -0.010809392387, -1.308837060762, 0.970778241189, 0.558946631235,
		11.389025401325, 12.830223040140, -8.517695939156, 0.293693134532, -0.845784968295, 1.418175236596, -2.246658259974,
		-9.885346240379, -8.517695939156, 10.222750966685, 0.390448668966, 1.095218945062, -0.403733435617, -1.815103304859,
		-0.010809392387, 0.293693134532, 0.390448668966, 2.256864603458, 0.632080300647, -0.019551779384, -1.751417951792,
		-1.308837060762, -0.845784968295, 1.095218945062, 0.632080300647, 3.132753467561, -1.427748733399, -4.291958669471,
		0.970778241189, 1.418175236596, -0.403733435617, -0.019551779384, -1.427748733399, 1.183055586213, 0.200571452172,
		0.558946631235, -2.246658259974, -1.815103304859, -1.751417951792, -4.291958669471, 0.200571452172, 11.668888615934
	})
},
{
	PLACE_CATEG_ID_OTHER,
	ctx::MahalModel(
	{
		-0.542340098504, 0.184789511765, 0.387451546413, 0.301902661472, 0.109392397093, -0.310468874039, 0.709513920221
	},
	{
		2.153884992301, -0.129488409324, 0.136236052776, -0.138043678532, -0.227492557156, 0.117810812390, 0.265072329266,
		-0.129488409324, 3.165213522741, -1.751520714507, 0.467831090302, -0.483916138161, 0.376293684450, 0.149387541935,
		0.136236052776, -1.751520714507, 2.483475248800, 0.384085303028, 0.338642175318, -0.052000492068, -0.801404345627,
		-0.138043678532, 0.467831090302, 0.384085303028, 1.972390458477, -0.025332052563, 0.393845805027, -1.225948397955,
		-0.227492557156, -0.483916138161, 0.338642175318, -0.025332052563, 0.890301343360, -0.549163112351, -0.746838701215,
		0.117810812390, 0.376293684450, -0.052000492068, 0.393845805027, -0.549163112351, 0.474674836872, 0.012417969474,
		0.265072329266, 0.149387541935, -0.801404345627, -1.225948397955, -0.746838701215, 0.012417969474, 2.104629121515
	})
}});

ctx::PiecewiseLin ctx::VisitCateger::__chiApprox(
{
	0.  ,  0.01,  0.02,  0.03,  0.04,  0.05,  0.06,  0.07,  0.08,
	0.09,  0.1 ,  0.11,  0.12,  0.13,  0.14,  0.15,  0.16,  0.17,
	0.18,  0.19,  0.2 ,  0.21,  0.22,  0.23,  0.24,  0.25,  0.26,
	0.27,  0.28,  0.29,  0.3 ,  0.31,  0.32,  0.33,  0.34,  0.35,
	0.36,  0.37,  0.38,  0.39,  0.4 ,  0.41,  0.42,  0.43,  0.44,
	0.45,  0.46,  0.47,  0.48,  0.49,  0.5 ,  0.51,  0.52,  0.53,
	0.54,  0.55,  0.56,  0.57,  0.58,  0.59,  0.6 ,  0.61,  0.62,
	0.63,  0.64,  0.65,  0.66,  0.67,  0.68,  0.69,  0.7 ,  0.71,
	0.72,  0.73,  0.74,  0.75,  0.76,  0.77,  0.78,  0.79,  0.8 ,
	0.81,  0.82,  0.83,  0.84,  0.85,  0.86,  0.87,  0.88,  0.89,
	0.9 ,  0.91,  0.92,  0.93,  0.94,  0.95,  0.96,  0.97,  0.98,
	0.99,  1.  ,  1.01,  1.02,  1.03,  1.04,  1.05,  1.06,  1.07,
	1.08,  1.09,  1.1 ,  1.11,  1.12,  1.13,  1.14,  1.15,  1.16,
	1.17,  1.18,  1.19,  1.2 ,  1.21,  1.22,  1.23,  1.24,  1.25,
	1.26,  1.27,  1.28,  1.29,  1.3 ,  1.31,  1.32,  1.33,  1.34,
	1.35,  1.36,  1.37,  1.38,  1.39,  1.4 ,  1.41,  1.42,  1.43,
	1.44,  1.45,  1.46,  1.47,  1.48,  1.49,  1.5 ,  1.51,  1.52,
	1.53,  1.54,  1.55,  1.56,  1.57,  1.58,  1.59,  1.6 ,  1.61,
	1.62,  1.63,  1.64,  1.65,  1.66,  1.67,  1.68,  1.69,  1.7 ,
	1.71,  1.72,  1.73,  1.74,  1.75,  1.76,  1.77,  1.78,  1.79,
	1.8 ,  1.81,  1.82,  1.83,  1.84,  1.85,  1.86,  1.87,  1.88,
	1.89,  1.9 ,  1.91,  1.92,  1.93,  1.94,  1.95,  1.96,  1.97,
	1.98,  1.99,  2.  ,  2.01,  2.02,  2.03,  2.04,  2.05,  2.06,
	2.07,  2.08,  2.09,  2.1 ,  2.11,  2.12,  2.13,  2.14,  2.15,
	2.16,  2.17,  2.18,  2.19,  2.2 ,  2.21,  2.22,  2.23,  2.24,
	2.25,  2.26,  2.27,  2.28,  2.29,  2.3 ,  2.31,  2.32,  2.33,
	2.34,  2.35,  2.36,  2.37,  2.38,  2.39,  2.4 ,  2.41,  2.42,
	2.43,  2.44,  2.45,  2.46,  2.47,  2.48,  2.49,  2.5 ,  2.51,
	2.52,  2.53,  2.54,  2.55,  2.56,  2.57,  2.58,  2.59,  2.6 ,
	2.61,  2.62,  2.63,  2.64,  2.65,  2.66,  2.67,  2.68,  2.69,
	2.7 ,  2.71,  2.72,  2.73,  2.74,  2.75,  2.76,  2.77,  2.78,
	2.79,  2.8 ,  2.81,  2.82,  2.83,  2.84,  2.85,  2.86,  2.87,
	2.88,  2.89,  2.9 ,  2.91,  2.92,  2.93,  2.94,  2.95,  2.96,
	2.97,  2.98,  2.99,  3.  ,  3.01,  3.02,  3.03,  3.04,  3.05,
	3.06,  3.07,  3.08,  3.09,  3.1 ,  3.11,  3.12,  3.13,  3.14,
	3.15,  3.16,  3.17,  3.18,  3.19,  3.2 ,  3.21,  3.22,  3.23,
	3.24,  3.25,  3.26,  3.27,  3.28,  3.29,  3.3 ,  3.31,  3.32,
	3.33,  3.34,  3.35,  3.36,  3.37,  3.38,  3.39,  3.4 ,  3.41,
	3.42,  3.43,  3.44,  3.45,  3.46,  3.47,  3.48,  3.49,  3.5 ,
	3.51,  3.52,  3.53,  3.54,  3.55,  3.56,  3.57,  3.58,  3.59,
	3.6 ,  3.61,  3.62,  3.63,  3.64,  3.65,  3.66,  3.67,  3.68,
	3.69,  3.7 ,  3.71,  3.72,  3.73,  3.74,  3.75,  3.76,  3.77,
	3.78,  3.79,  3.8 ,  3.81,  3.82,  3.83,  3.84,  3.85,  3.86,
	3.87,  3.88,  3.89,  3.9 ,  3.91,  3.92,  3.93,  3.94,  3.95,
	3.96,  3.97,  3.98,  3.99,  4.  ,  4.01,  4.02,  4.03,  4.04,
	4.05,  4.06,  4.07,  4.08,  4.09,  4.1 ,  4.11,  4.12,  4.13,
	4.14,  4.15,  4.16,  4.17,  4.18,  4.19,  4.2 ,  4.21,  4.22,
	4.23,  4.24,  4.25,  4.26,  4.27,  4.28,  4.29,  4.3 ,  4.31,
	4.32,  4.33,  4.34,  4.35,  4.36,  4.37,  4.38,  4.39,  4.4 ,
	4.41,  4.42,  4.43,  4.44,  4.45,  4.46,  4.47,  4.48,  4.49,
	4.5 ,  4.51,  4.52,  4.53,  4.54,  4.55,  4.56,  4.57,  4.58,
	4.59,  4.6 ,  4.61,  4.62,  4.63,  4.64,  4.65,  4.66,  4.67,
	4.68,  4.69,  4.7 ,  4.71,  4.72,  4.73,  4.74,  4.75,  4.76,
	4.77,  4.78,  4.79,  4.8 ,  4.81,  4.82,  4.83,  4.84,  4.85,
	4.86,  4.87,  4.88,  4.89,  4.9 ,  4.91,  4.92,  4.93,  4.94,
	4.95,  4.96,  4.97,  4.98,  4.99,  5.  ,  5.01,  5.02,  5.03,
	5.04,  5.05,  5.06,  5.07,  5.08,  5.09,  5.1 ,  5.11,  5.12,
	5.13,  5.14,  5.15,  5.16,  5.17,  5.18,  5.19,  5.2 ,  5.21,
	5.22,  5.23,  5.24,  5.25,  5.26,  5.27,  5.28,  5.29,  5.3 ,
	5.31,  5.32,  5.33,  5.34,  5.35,  5.36,  5.37,  5.38,  5.39,
	5.4 ,  5.41,  5.42,  5.43,  5.44,  5.45,  5.46,  5.47,  5.48,
	5.49,  5.5 ,  5.51,  5.52,  5.53,  5.54,  5.55,  5.56,  5.57,
	5.58,  5.59,  5.6 ,  5.61,  5.62,  5.63,  5.64,  5.65,  5.66,
	5.67,  5.68,  5.69,  5.7 ,  5.71,  5.72,  5.73,  5.74,  5.75,
	5.76,  5.77,  5.78,  5.79,  5.8 ,  5.81,  5.82,  5.83,  5.84,
	5.85,  5.86,  5.87,  5.88,  5.89,  5.9 ,  5.91,  5.92,  5.93,
	5.94,  5.95,  5.96,  5.97,  5.98,  5.99,  6.  ,  6.01,  6.02,
	6.03,  6.04,  6.05,  6.06,  6.07,  6.08,  6.09,  6.1 ,  6.11,
	6.12,  6.13,  6.14,  6.15,  6.16,  6.17,  6.18,  6.19,  6.2 ,
	6.21,  6.22,  6.23,  6.24,  6.25,  6.26,  6.27,  6.28,  6.29,
	6.3 ,  6.31,  6.32,  6.33,  6.34,  6.35,  6.36,  6.37,  6.38,
	6.39,  6.4 ,  6.41,  6.42,  6.43,  6.44,  6.45,  6.46,  6.47,
	6.48,  6.49,  6.5 ,  6.51,  6.52,  6.53,  6.54,  6.55,  6.56,
	6.57,  6.58,  6.59,  6.6 ,  6.61,  6.62,  6.63,  6.64,  6.65,
	6.66,  6.67,  6.68,  6.69,  6.7 ,  6.71,  6.72,  6.73,  6.74,
	6.75,  6.76,  6.77,  6.78,  6.79,  6.8 ,  6.81,  6.82,  6.83,
	6.84,  6.85,  6.86,  6.87,  6.88,  6.89,  6.9 ,  6.91,  6.92,
	6.93,  6.94,  6.95,  6.96,  6.97,  6.98,  6.99,  7.
},
{
	0.      ,  0.      ,  0.      ,  0.      ,  0.      ,  0.      ,
	0.      ,  0.      ,  0.      ,  0.      ,  0.      ,  0.      ,
	0.      ,  0.      ,  0.      ,  0.      ,  0.      ,  0.      ,
	0.      ,  0.      ,  0.      ,  0.      ,  0.      ,  0.      ,
	0.      ,  0.      ,  0.000001,  0.000001,  0.000001,  0.000001,
	0.000002,  0.000002,  0.000003,  0.000003,  0.000004,  0.000005,
	0.000006,  0.000007,  0.000008,  0.00001 ,  0.000012,  0.000014,
	0.000016,  0.000019,  0.000023,  0.000026,  0.000031,  0.000035,
	0.000041,  0.000047,  0.000054,  0.000062,  0.00007 ,  0.00008 ,
	0.000091,  0.000103,  0.000116,  0.000131,  0.000147,  0.000165,
	0.000185,  0.000207,  0.000231,  0.000257,  0.000285,  0.000316,
	0.00035 ,  0.000387,  0.000427,  0.000471,  0.000518,  0.000569,
	0.000624,  0.000683,  0.000747,  0.000816,  0.00089 ,  0.00097 ,
	0.001055,  0.001147,  0.001245,  0.001349,  0.001461,  0.00158 ,
	0.001708,  0.001843,  0.001987,  0.002141,  0.002303,  0.002476,
	0.002659,  0.002854,  0.003059,  0.003276,  0.003506,  0.003748,
	0.004004,  0.004274,  0.004558,  0.004857,  0.005171,  0.005502,
	0.00585 ,  0.006215,  0.006597,  0.006999,  0.007419,  0.007859,
	0.00832 ,  0.008802,  0.009305,  0.009831,  0.01038 ,  0.010953,
	0.01155 ,  0.012172,  0.01282 ,  0.013495,  0.014197,  0.014927,
	0.015686,  0.016473,  0.017291,  0.01814 ,  0.019021,  0.019933,
	0.020879,  0.021858,  0.022872,  0.023921,  0.025006,  0.026127,
	0.027285,  0.028482,  0.029717,  0.030992,  0.032306,  0.033662,
	0.035059,  0.036497,  0.037979,  0.039504,  0.041073,  0.042687,
	0.044345,  0.04605 ,  0.047801,  0.049599,  0.051445,  0.053339,
	0.055282,  0.057273,  0.059315,  0.061406,  0.063548,  0.065742,
	0.067986,  0.070283,  0.072632,  0.075034,  0.077488,  0.079996,
	0.082558,  0.085174,  0.087843,  0.090568,  0.093346,  0.09618 ,
	0.099069,  0.102013,  0.105012,  0.108066,  0.111176,  0.114342,
	0.117563,  0.120839,  0.124171,  0.127558,  0.131001,  0.134499,
	0.138052,  0.141659,  0.145322,  0.149039,  0.15281 ,  0.156635,
	0.160514,  0.164446,  0.168431,  0.172469,  0.176559,  0.180701,
	0.184894,  0.189138,  0.193432,  0.197776,  0.202169,  0.206611,
	0.211101,  0.215639,  0.220223,  0.224853,  0.229528,  0.234248,
	0.239012,  0.24382 ,  0.248669,  0.25356 ,  0.258492,  0.263464,
	0.268474,  0.273523,  0.278608,  0.283731,  0.288888,  0.29408 ,
	0.299305,  0.304562,  0.309851,  0.31517 ,  0.320519,  0.325895,
	0.331299,  0.336729,  0.342185,  0.347664,  0.353166,  0.35869 ,
	0.364234,  0.369798,  0.375381,  0.38098 ,  0.386596,  0.392227,
	0.397871,  0.403529,  0.409197,  0.414876,  0.420565,  0.426261,
	0.431964,  0.437673,  0.443387,  0.449103,  0.454822,  0.460543,
	0.466263,  0.471981,  0.477698,  0.483411,  0.48912 ,  0.494822,
	0.500518,  0.506206,  0.511886,  0.517554,  0.523212,  0.528858,
	0.53449 ,  0.540108,  0.545711,  0.551297,  0.556866,  0.562417,
	0.567948,  0.573459,  0.578949,  0.584417,  0.589861,  0.595282,
	0.600677,  0.606048,  0.611391,  0.616707,  0.621995,  0.627254,
	0.632483,  0.637681,  0.642848,  0.647984,  0.653086,  0.658155,
	0.66319 ,  0.66819 ,  0.673155,  0.678084,  0.682976,  0.687831,
	0.692649,  0.697428,  0.702168,  0.70687 ,  0.711531,  0.716153,
	0.720733,  0.725273,  0.729772,  0.734228,  0.738643,  0.743015,
	0.747344,  0.75163 ,  0.755873,  0.760072,  0.764227,  0.768339,
	0.772406,  0.776428,  0.780406,  0.784339,  0.788228,  0.792071,
	0.795869,  0.799622,  0.80333 ,  0.806992,  0.810609,  0.814181,
	0.817707,  0.821188,  0.824624,  0.828015,  0.83136 ,  0.83466 ,
	0.837916,  0.841126,  0.844292,  0.847413,  0.85049 ,  0.853522,
	0.85651 ,  0.859455,  0.862355,  0.865212,  0.868026,  0.870796,
	0.873524,  0.876209,  0.878852,  0.881452,  0.884011,  0.886528,
	0.889005,  0.89144 ,  0.893834,  0.896189,  0.898503,  0.900778,
	0.903014,  0.90521 ,  0.907369,  0.909488,  0.911571,  0.913615,
	0.915623,  0.917594,  0.919528,  0.921427,  0.92329 ,  0.925118,
	0.926911,  0.92867 ,  0.930395,  0.932086,  0.933745,  0.93537 ,
	0.936963,  0.938525,  0.940055,  0.941554,  0.943022,  0.94446 ,
	0.945869,  0.947248,  0.948598,  0.949919,  0.951213,  0.952478,
	0.953717,  0.954928,  0.956113,  0.957273,  0.958406,  0.959514,
	0.960598,  0.961657,  0.962692,  0.963703,  0.964692,  0.965657,
	0.9666  ,  0.967521,  0.968421,  0.969299,  0.970156,  0.970993,
	0.97181 ,  0.972607,  0.973385,  0.974144,  0.974884,  0.975605,
	0.976309,  0.976996,  0.977665,  0.978317,  0.978953,  0.979572,
	0.980176,  0.980764,  0.981337,  0.981895,  0.982438,  0.982967,
	0.983482,  0.983984,  0.984472,  0.984947,  0.985409,  0.985858,
	0.986296,  0.986721,  0.987135,  0.987537,  0.987929,  0.988309,
	0.988678,  0.989038,  0.989387,  0.989726,  0.990056,  0.990376,
	0.990687,  0.990989,  0.991282,  0.991566,  0.991843,  0.992111,
	0.992371,  0.992624,  0.992869,  0.993106,  0.993337,  0.99356 ,
	0.993777,  0.993988,  0.994191,  0.994389,  0.99458 ,  0.994766,
	0.994946,  0.99512 ,  0.995289,  0.995452,  0.99561 ,  0.995764,
	0.995912,  0.996056,  0.996195,  0.996329,  0.99646 ,  0.996586,
	0.996708,  0.996825,  0.996939,  0.99705 ,  0.997156,  0.99726 ,
	0.997359,  0.997456,  0.997549,  0.997639,  0.997726,  0.99781 ,
	0.997891,  0.997969,  0.998045,  0.998118,  0.998189,  0.998257,
	0.998323,  0.998386,  0.998447,  0.998506,  0.998563,  0.998618,
	0.998671,  0.998723,  0.998772,  0.998819,  0.998865,  0.998909,
	0.998952,  0.998993,  0.999033,  0.999071,  0.999107,  0.999143,
	0.999177,  0.99921 ,  0.999241,  0.999272,  0.999301,  0.999329,
	0.999356,  0.999382,  0.999407,  0.999431,  0.999455,  0.999477,
	0.999498,  0.999519,  0.999539,  0.999558,  0.999576,  0.999594,
	0.999611,  0.999627,  0.999643,  0.999658,  0.999672,  0.999686,
	0.999699,  0.999712,  0.999724,  0.999736,  0.999747,  0.999758,
	0.999769,  0.999779,  0.999788,  0.999797,  0.999806,  0.999815,
	0.999823,  0.99983 ,  0.999838,  0.999845,  0.999852,  0.999858,
	0.999865,  0.999871,  0.999876,  0.999882,  0.999887,  0.999892,
	0.999897,  0.999902,  0.999906,  0.99991 ,  0.999915,  0.999918,
	0.999922,  0.999926,  0.999929,  0.999932,  0.999936,  0.999938,
	0.999941,  0.999944,  0.999947,  0.999949,  0.999952,  0.999954,
	0.999956,  0.999958,  0.99996 ,  0.999962,  0.999964,  0.999965,
	0.999967,  0.999969,  0.99997 ,  0.999972,  0.999973,  0.999974,
	0.999975,  0.999977,  0.999978,  0.999979,  0.99998 ,  0.999981,
	0.999982,  0.999983,  0.999984,  0.999984,  0.999985,  0.999986,
	0.999987,  0.999987,  0.999988,  0.999988,  0.999989,  0.99999 ,
	0.99999 ,  0.999991,  0.999991,  0.999992,  0.999992,  0.999992,
	0.999993,  0.999993,  0.999993,  0.999994,  0.999994,  0.999994,
	0.999995,  0.999995,  0.999995,  0.999995,  0.999996,  0.999996,
	0.999996,  0.999996,  0.999997,  0.999997,  0.999997,  0.999997,
	0.999997,  0.999997,  0.999998,  0.999998,  0.999998,  0.999998,
	0.999998,  0.999998,  0.999998,  0.999998,  0.999998,  0.999998,
	0.999999,  0.999999,  0.999999,  0.999999,  0.999999,  0.999999,
	0.999999,  0.999999,  0.999999,  0.999999,  0.999999,  0.999999,
	0.999999,  0.999999,  0.999999,  0.999999,  0.999999,  0.999999,
	0.999999,  1.      ,  1.      ,  1.      ,  1.      ,  1.      ,
	1.      ,  1.      ,  1.      ,  1.      ,  1.      ,  1.      ,
	1.      ,  1.      ,  1.      ,  1.      ,  1.      ,  1.      ,
	1.      ,  1.      ,  1.      ,  1.      ,  1.      ,  1.      ,
	1.      ,  1.      ,  1.      ,  1.      ,  1.      ,  1.      ,
	1.      ,  1.      ,  1.      ,  1.      ,  1.      ,  1.      ,
	1.      ,  1.      ,  1.      ,  1.      ,  1.      ,  1.      ,
	1.      ,  1.      ,  1.      ,  1.      ,  1.      ,  1.      ,
	1.      ,  1.      ,  1.      ,  1.      ,  1.
} // this is chi cdf for 7 degrees of freedom (because we have 7 features)
);

const std::vector<ctx::num_t> ctx::VisitCateger::__featuresMean(
{
	344.542975696503,
	894.178423236515,
	868.300533491405,
	2.820391227030,
	0.511747454052,
	0.348669092762,
	0.139583453187
});

const std::vector<ctx::num_t> ctx::VisitCateger::__featuresStd(
{
	416.061437196941,
	301.401812101781,
	300.774466281622,
	1.916233112930,
	0.314254748759,
	0.360707461975,
	0.109386661911
});

ctx::TimeFeatures ctx::VisitCateger::__timeFeatures(const time_t &time)
{
	struct tm timeinfo;
	struct tm *result;
	tzset();
	result = localtime_r(&time, &timeinfo);
	if (result == NULL)
		return {0, 0, 0, false};
	int minutesSinceMidnight = 60 * timeinfo.tm_hour + timeinfo.tm_min;
	int weekday = (timeinfo.tm_wday + 6) % 7; // Monday is 0, Sunday is 6
	bool weekend = weekday > 4;
	int minutesSinceBeginingOfTheWeek = 24 * 60 * weekday + minutesSinceMidnight;
	return {
		minutesSinceMidnight,
		minutesSinceBeginingOfTheWeek,
		weekday,
		weekend
	};
}

int ctx::VisitCateger::__weeksScope(const TimeFeatures &startF, const Interval &interval)
{
	int durationMinutes = (interval.end - interval.start) / 60;
	int scopeMinutes = startF.minutesSinceBeginingOfTheWeek + durationMinutes;
	int weeksScope = scopeMinutes / __MINUTES_IN_WEEK;
	if (scopeMinutes % __MINUTES_IN_WEEK > 0)
		weeksScope++;
	return weeksScope;
}

ctx::num_t ctx::VisitCateger::__sum(const std::vector<num_t> model, const size_t &from, const size_t &to)
{
	size_t toSecure = to;
	if (to >= model.size()) {
		_E("'to' exceeds model size");
		toSecure = model.size() - 1;
	}
	if (from > to)
		_E("'from' greater than 'to'");
	num_t result = 0.0;
	for (size_t i = from; i <= toSecure; i++) {
		result += model[i];
	}
	return result;
}

ctx::num_t ctx::VisitCateger::__weekModelMeanValue(PlaceCategId categ, const Interval &interval,
		const TimeFeatures &startF, const TimeFeatures &endF)
{
	num_t ret = 0.0;
	int minutes = 0;
	int ws = __weeksScope(startF, interval);
	for (int week = 1; week <= ws; week++) {
		size_t startIndex = (week == 1)
				? startF.minutesSinceBeginingOfTheWeek
				: 0;
		size_t endIndex = (week == ws)
				? endF.minutesSinceBeginingOfTheWeek
				: __MINUTES_IN_WEEK - 1;
		ret += __sum(prob_features::weekModel[categ], startIndex, endIndex);
		minutes += endIndex - startIndex + 1;
	}
	if (minutes > 0)
		return ret / minutes;
	return ret;
}

ctx::Categs ctx::VisitCateger::__weekModelFeatures(const Interval &interval, const TimeFeatures &startF, const TimeFeatures &endF)
{
	ctx::Categs categs;
	for (const auto &item : prob_features::weekModel) {
		categs[item.first] = __weekModelMeanValue(item.first, interval, startF, endF);
	}
	_D("categs: H=%.12f, W=%.12f, O=%.12f",
			categs[PLACE_CATEG_ID_HOME],
			categs[PLACE_CATEG_ID_WORK],
			categs[PLACE_CATEG_ID_OTHER]);
	return categs;
}

ctx::IntervalFeatures ctx::VisitCateger::__intervalFeatures(const Interval &interval)
{
	num_t durationMinutes = 1.0 * (interval.end - interval.start) / 60;
	TimeFeatures startFeatures = __timeFeatures(interval.start);
	TimeFeatures endFeatures = __timeFeatures(interval.end);
	Categs weekFeatures = __weekModelFeatures(interval, startFeatures, endFeatures);
	return {
		durationMinutes,
		(num_t) startFeatures.minutesSinceMidnight,
		(num_t) endFeatures.minutesSinceMidnight,
		(num_t) startFeatures.weekday,
		weekFeatures[PLACE_CATEG_ID_HOME],
		weekFeatures[PLACE_CATEG_ID_WORK],
		weekFeatures[PLACE_CATEG_ID_OTHER]
	};
}

void ctx::VisitCateger::__normalize(std::vector<ctx::num_t> &features)
{
	size_t n = features.size();
	for (size_t i = 0; i < n; i++) {
		features[i] -= __featuresMean[i];
		features[i] /= __featuresStd[i];
	}
}

void ctx::VisitCateger::categorize(ctx::Visit &visit)
{
	IntervalFeatures features = __intervalFeatures(visit.interval);
	__normalize(features);

	for (auto &modelPair : __models) {
		int categId = modelPair.first;
		MahalModel model = modelPair.second;

		num_t distance = model.distance(features);
		num_t probability = 1 - __chiApprox.value(distance); // sth like probability but not exactly
		visit.categs[categId] = probability;
	}
}

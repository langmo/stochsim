#pragma once

/// <summary>
/// Parameters of phage infection dynamics.
/// Note that some parameters are given in concentration units, and have to be converted to absolute numbers given a certain reaction volume
/// before using them in stochastic simulations.
/// </summary>
struct Parameters
{
	/// <summary>
	/// max growth rate [h^-1]
	/// </summary>
	static constexpr double v_max = 0.55;
	/// <summary>
	/// phage absorbtion rate [ml / h]
	//	note that this parameter is concentration based. To use it in a simulation, divide by the volume, i.e. use delta/V.
	/// </summary>
	static constexpr double delta = 4.1e-8;
	/// <summary>
	/// total latent period for lysis, i.e. time between infection and lysis, if fate decision turns out to be lysis [h]
	/// </summary>
	static constexpr double totalLysisPeriod = 1.;
	/// <summary>
	/// period during which additional infections influence the fate decision [h]
	/// </summary>
	static constexpr double moiPeriod = 5./60.;
	/// <summary>
	/// period required to lyse after cell fate decision to lyse has been made [h]
	/// </summary>
	static constexpr double lysisPeriod = totalLysisPeriod - moiPeriod;
	/// <summary>
	/// burst size [phages]
	/// </summary>
	static constexpr long beta = 63; 
	/// <summary>
	/// phage escape probability for EcoRI [-]
	/// </summary>
	static constexpr double mu_EcoRI = 1.37e-5;
	/// <summary>
	/// phage escape probability for EcoRV [-]
	/// </summary>
	static constexpr double mu_EcoRV = 1.96e-8;
	/// <summary>
	/// induction rate [1 / h]
	/// </summary>
	static constexpr double xi = 2.4e-5;
	/// <summary>
	/// probability of lysogeny [-]
	/// </summary>
	static constexpr double alpha = 0.02;
};
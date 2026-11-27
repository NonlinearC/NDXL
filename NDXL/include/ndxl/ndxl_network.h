/**
 * @file ndxl_network.h
 *
 * @brief Public API for NDXL network management and simulation.
 *
 * Declares the NDXLNetwork object and provides functions for network construction, destruction, traversal, parameter management, and numerical simulation.
 *
 * @copyright Copyright (c) 2026, Ethan Codron.
 *
 * Licensed under the GNU General Public License v3.0 only.
 * See the LICENSE file in the repository root.
*/

#ifndef _NDXL_NETWORK_
#define _NDXL_NETWORK_

#include "../ndxlcore.h"

/**
 * @defgroup network_callbacks Callbacks
 * @ingroup ndxl_network
 * @brief Callback signatures for networks during simulation.
 *
 * @{
*/

/**
 * @brief Network callback function.
 *
 * @details Defines a user-supplied discrete callback that is invoked during network simulation.
 *
 * @param[in] _Network
 * Pointer to the network being simulated.
 *
 * @param[in] _Time
 * Current local simulation time of the network.
 *
 * @param[in] _Params
 * User-defined parameters associated with the network.
 *
 * @return
 * Integer indicating callback success (zero) or failure (any nonzero value).
 *
 * @note
 * If this function returns a nonzero value during simulation, NDXL terminates the simulation, and internally raises a @ref NDXL_SIM_ERR.
*/
typedef int(*NDXLNetworkCallback)(NDXLNetwork* _Network, double _Time, void* _Params);

/** @}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup network_creation Constructor and Destructor
 * @ingroup ndxl_network
 * @brief Functions for creating and destroying networks.
 *
 * @{
*/

/**
 * @brief Creates an empty NDXLNetwork object and returns a pointer to it.
 *
 * @param[in] _Params
 * Pointer to extra memory used by the network. May be NULL.
 *
 * @return
 * If successful, returns a new NDXLNetwork pointer, otherwise returns NULL.
 *
 * @note
 * The local simulation time of a newly created network is always initialised 0, and only updates on a successful integration step.
 * 
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the network uses it. The user remains responsible for managing any allocations.
 *
 * @see ndxlDestroyNetwork
 * @see ndxlGetNetworkTime
 * @see ndxlNetworkSimulateRalston
 * @see ndxlNetworkSimulateNystrom
 * @see ndxlNetworkSimulateDopri
 * @see ndxlNetworkSimulateRkck
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
 * @see ndxlNetworkSimulateRadau
*/
_NDXLIMP_ NDXLNetwork* _CDECL ndxlCreateNetwork(
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/**
 * @brief Destroys a network created using ndxlCreateNetwork(), and zeroes the pointer.
 *
 * @param[in,out] _Network
 * Pointer to NDXLNetwork pointer.
 *
 * @return
 * If _Network is NULL, returns @ref NDXL_ERR, otherwise destroys the network and returns NDXL_OK.
 * 
 * @warning
 * Destroying a network also destroys all vertices, edges, and noise channels owned by that network. All pointers to the network and its owned objects become invalid. The memory associated with the relevant objects' user-defined `void* _Params` pointers is not handled by the destructors, and must be managed explicitly by the user.
 *
 * @warning
 * Using the network after it has been destroyed can result in undefined behaviour. Discard all references!!
 * 
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * 
 * @see ndxlCreateNetwork
*/
_NDXLIMP_ const int _CDECL ndxlDestroyNetwork(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup network_getters Getters
 * @ingroup ndxl_network
 * @brief Getter functions for querying network properties.
 *
 * @{
*/

/**
 * @brief Gets the number of noise channels in a network.
 *
 * @param[in] _Network
 * Target network to retrieve the noise dimension of.
 *
 * @return
 * If _Network is NULL, returns @ref NDXL_ERR, otherwise returns the number of noise channels in _Network.
 *
 * @note
 * Not to be confused with ndxlGetVertexNoiseDimension and ndxlGetEdgeNoiseDimension, which return the number of noise channels affecting a vertex and edge, respectively.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 *
 * @see ndxlGetVertexNoiseDimension
 * @see ndxlGetEdgeNoiseDimension
*/
_NDXLIMP_ const size_t _CDECL ndxlGetNetworkNoiseDimension(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the number of vertices in a network.
 *
 * @param[in] _Network
 * Target network to retrieve the order of.
 *
 * @return
 * If _Network is NULL, returns @ref NDXL_ERR, otherwise returns the number of vertices in _Network.
 *
 * @note
 * Not to be confused with ndxlGetNetworkSize, which returns the number of edges in a network.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 *
 * @see ndxlGetNetworkSize
*/
_NDXLIMP_ const size_t _CDECL ndxlGetNetworkOrder(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the user-defined `void* _Params` of a network.
 *
 * @param[in] _Network
 * Target network to retrive the parameters of.
 *
 * @return
 * If _Network is NULL, returns NULL, otherwise returns the user-defined `void* _Params` of _Network.
 * 
 * @note
 * A return value of NULL does not necessarily indicate an error, as `void* _Params` could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetNetworkParams
*/
_NDXLIMP_ void* _CDECL ndxlGetNetworkParams(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the number of edges in a network.
 *
 * @param[in] _Network
 * Target network to retrive the size of.
 *
 * @return
 * If _Network is NULL, returns @ref NDXL_ERR, otherwise returns the number of edges in _Network.
 *
 * @note
 * Not to be confused with ndxlGetNetworkOrder, which returns the number of vertices in a network.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 *
 * @see ndxlGetNetworkOrder
*/
_NDXLIMP_ const size_t _CDECL ndxlGetNetworkSize(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/**
 * @brief Gets the local simulation time of a network.
 *
 * @param[in] _Network
 * Target network to retrive the time of.
 *
 * @return
 * If _Network is NULL, returns @ref NDXL_ERR, otherwise returns the local simulation time of _Network.
 *
 * @note
 * The local simulation time only updates on a successful simulation step.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
*/
_NDXLIMP_ const double _CDECL ndxlGetNetworkTime(
	_In_ _Notnull_ NDXLNetwork* _Network
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup network_setters Setters
 * @ingroup ndxl_network
 * @brief Setter functions for updating network properties.
 *
 * @{
*/

/**
 * @brief Sets the user-defined `void* _Params` of a network.
 *
 * @param[in] _Network
 * Target network to update the parameters of.
 *
 * @param[in] _Params
 * Pointer to extra memory used by the network. May be NULL.
 *
 * @return
 * If _Network is NULL, returns @ref NDXL_ERR, otherwise updates the parameters of _Network, and returns NDXL_OK.
 *
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the network uses it.
 *
 * @warning
 * This function replaces the networks's existing parameter pointer without freeing or otherwise managing the memory it previously referenced. The user remains responsible for managing both the previous and new allocations.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 *
 * @see ndxlGetNetworkParams
*/
_NDXLIMP_ const int _CDECL ndxlSetNetworkParams(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup network_functions Functions
 * @ingroup ndxl_network
 * @brief Functions for operating on networks.
 *
 * @{
*/

/**
 * @brief Provides directed access to an channel in a network.
 *
 * @param[in] _Network
 * Target network to retrieve the channel in.
 *
 * @param[in] _Idx
 * Relative index (position) of channel in the array. Must be greater than or equal to zero, and less than ndxlGetNetworkNoiseDimension(_Network).
 *
 * @return
 * If _Network is NULL or contains no channels, or _Idx is out of bounds of the underlying array, returns NULL, otherwise returns NDXLChannel pointer currently at position _Idx in the network's underlying channel array.
 *
 * @note
 * The pointer refers to the original object in the network, so modifications through it affect the object directly. The returned object can be used to modify or destroy its referenced object without invalidating the object itself.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the network could have no channels. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @warning
 * Relative indices are shuffled when agents are destroyed to save memory. Repeated calls with the same index will not necessarily return the same object.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : _Idx is out of bounds.
 *
 * @see ndxlGetNetworkNoiseDimension
 * @see ndxlNetworkChannelByIndex
*/
_NDXLIMP_ NDXLChannel* _CDECL ndxlNetworkChannel(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const size_t _Idx
) _NOEXCEPT;

/**
 * @brief Provides directed access to an channel in a network.
 *
 * @param[in] _Network
 * Target network to retrieve the channel in.
 *
 * @param[in] _Idx
 * Absolute index (id) of channel in the array. Equivalent to calling ndxlGetChannelIndex(_Channel).
 *
 * @return
 * If _Network is NULL or contains no channels, or no channel exists with the id _Idx, returns NULL, otherwise returns NDXLChannel pointer currently in the network's underlying channel array whose id is _Idx.
 *
 * @note
 * The pointer refers to the original object in the network, so modifications through it affect the object directly. The returned object can be used to modify or destroy its referenced object without invalidating the object itself.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : No channel with id _Idx was found.
 *
 * @see ndxlGetChannelIndex
 * @see ndxlNetworkChannel
*/
_NDXLIMP_ NDXLChannel* _CDECL ndxlNetworkChannelByIndex(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const size_t _Idx
) _NOEXCEPT;

/**
 * @brief Provides directed access to an edge in a network.
 *
 * @param[in] _Network
 * Target network to retrieve the edge in.
 *
 * @param[in] _Idx
 * Relative index (position) of edge in the array. Must be greater than or equal to zero, and less than ndxlGetNetworkSize(_Network).
 *
 * @return
 * If _Network is NULL or contains no edges, or _Idx is out of bounds of the underlying array, returns NULL, otherwise returns NDXLEdge pointer currently at position _Idx in the network's underlying edge array.
 *
 * @note
 * The pointer refers to the original object in the network, so modifications through it affect the object directly. The returned object can be used to modify or destroy its referenced object without invalidating the object itself.
 * 
 * @note
 * A return value of NULL does not necessarily indicate an error, as the network could have no edges. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @warning
 * Relative indices are shuffled when agents are destroyed to save memory. Repeated calls with the same index will not necessarily return the same object.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : _Idx is out of bounds.
 *
 * @see ndxlGetNetworkSize
 * @see ndxlNetworkEdgeByIndex
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlNetworkEdge(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const size_t _Idx
) _NOEXCEPT;

/**
 * @brief Provides directed access to an edge in a network.
 *
 * @param[in] _Network
 * Target network to retrieve the edge in.
 *
 * @param[in] _Idx
 * Absolute index (id) of edge in the array. Equivalent to calling ndxlGetEdgeIndex(_Edge).
 *
 * @return
 * If _Network is NULL or contains no edges, or no edge exists with the id _Idx, returns NULL, otherwise returns NDXLEdge pointer currently in the network's underlying edge array whose id is _Idx.
 *
 * @note
 * The pointer refers to the original object in the network, so modifications through it affect the object directly. The returned object can be used to modify or destroy its referenced object without invalidating the object itself.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : No edge with id _Idx was found.
 *
 * @see ndxlGetEdgeIndex
 * @see ndxlNetworkEdge
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlNetworkEdgeByIndex(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const size_t _Idx
) _NOEXCEPT;

/**
 * @brief Provides directed access to an vertex in a network.
 *
 * @param[in] _Network
 * Target network to retrieve the vertex in.
 *
 * @param[in] _Idx
 * Relative index (position) of vertex in the array. Must be greater than or equal to zero, and less than ndxlGetNetworkOrder(_Network).
 *
 * @return
 * If _Network is NULL or contains no vertices, or _Idx is out of bounds of the underlying array, returns NULL, otherwise returns NDXLVertex pointer currently at position _Idx in the network's underlying vertex array.
 *
 * @note
 * The pointer refers to the original object in the network, so modifications through it affect the object directly. The returned object can be used to modify or destroy its referenced object without invalidating the object itself.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the network could have no vertices. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @warning
 * Relative indices are shuffled when agents are destroyed to save memory. Repeated calls with the same index will not necessarily return the same object.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : _Idx is out of bounds.
 *
 * @see ndxlGetNetworkOrder
 * @see ndxlNetworkVertexByIndex
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlNetworkVertex(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const size_t _Idx
) _NOEXCEPT;

/**
 * @brief Provides directed access to an vertex in a network.
 *
 * @param[in] _Network
 * Target network to retrieve the vertex in.
 *
 * @param[in] _Idx
 * Absolute index (id) of vertex in the array. Equivalent to calling ndxlGetVertexIndex(_Vertex).
 *
 * @return
 * If _Network is NULL or contains no vertices, or no vertex exists with the id _Idx, returns NULL, otherwise returns NDXLVertex pointer currently in the network's underlying vertex array whose id is _Idx.
 *
 * @note
 * The pointer refers to the original object in the network, so modifications through it affect the object directly. The returned object can be used to modify or destroy its referenced object without invalidating the object itself.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : No vertex with id _Idx was found.
 *
 * @see ndxlGetVertexIndex
 * @see ndxlNetworkVertex
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlNetworkVertexByIndex(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const size_t _Idx
) _NOEXCEPT;

/** @}*/


/**
 * @defgroup network_simulation Simulation
 * @ingroup ndxl_network
 * @brief Functions for simulating networks.
 *
 * @par Simulation Behaviour
 * Simulation starts at the local simulation time of a network i.e., the first simulation starts at t = 0, and subsequent simulations on the same network start at whatever time the previous simulation finished.
 * Each network essentially functions as its own state machine, and because networks can dynamically change during simulation, only the current simulation state is preserved. Any errors encountered during simulation thus leave the network in the state it was in before the error occured.
 * The evaluation order for a single Runge-Kutta stage is all vertices from ndxlGetNetworkFirstVertex to ndxlGetNetworkLastVertex, followed by all edges from ndxlGetNetworkFirstEdge to ndxlGetNetworkLastEdge.
 *
 * @par Callback Invocation
 * Network callbacks are invoked once at local simulation time t = 0, before integration begins, and subsequently after every successful integration step. The initial callback is only invoked once during the lifetime of a network; repeated calls to simulation functions do not invoke it again.
 * Agent callbacks are invoked after every successful integration step, but are not invoked at the initial simulation time. Like stepping, vertex callbacks are evaluated first, then edge callbacks, in the same order as described above.
 *
 * @par External Failure
 * Any callback, dynamics, or noise function returning any nonzero value indicates failure. NDXL immediately terminates the simulation upon receiving this signal.
 * The network state is not rolled back following a callback failure. Instead, it remains at the state reached when the callback was invoked, with all previously completed integration steps and the network configuration preserved.
 *
 * @see ndxlGetNetworkVertices
 * @see ndxlGetNetworkVertices
 * @see ndxlGetNetworkEdges
 * @see ndxlGetNetworkEdges
 * @see NDXLNetworkCallback
 * @see NDXLVertexDynamics
 * @see NDXLVertexNoise
 * @see NDXLVertexCallback
 * @see NDXLEdgeDynamics
 * @see NDXLEdgeNoise
 * @see NDXLEdgeCallback
 *
 * @{
*/

/**
 * @brief Simulates a network using the static explicit Ralston method of order 4.
 *
 * @param[in] _Network
 * Target network to simulate.
 *
 * @param[in] _Time
 * Duration of the simulation. Must be greater than zero.
 *
 * @param[in] _Step
 * Step size. Must be greater than zero.
 *
 * @param[in] _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns @ref NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains stochastic agents as it is a purely deterministic method.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : _Time <= 0.
 * - @ref NDXL_VALUE_ERR : _Step <= 0.
 * - @ref NDXL_SIM_ERR : _Network contains no agents.
 * - @ref NDXL_SIM_ERR : _Network contains stochastic agents, one of the stochastic methods should be used.
 * - @ref NDXL_SIM_ERR : A dynamics, noise, or callback function returned a nonzero value during simulation.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateRalston(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Step,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/**
 * @brief Simulates a network using the static explicit Nystr&ouml;m method of order 5.
 *
 * @param[in] _Network
 * Target network to simulate.
 *
 * @param[in] _Time
 * Duration of the simulation. Must be greater than zero.
 *
 * @param[in] _Step
 * Step size. Must be greater than zero.
 *
 * @param[in] _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns @ref NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains stochastic agents as it is a purely deterministic method.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : _Time <= 0.
 * - @ref NDXL_VALUE_ERR : _Step <= 0.
 * - @ref NDXL_SIM_ERR : _Network contains no agents.
 * - @ref NDXL_SIM_ERR : _Network contains stochastic agents, one of the stochastic methods should be used.
 * - @ref NDXL_SIM_ERR : A dynamics, noise, or callback function returned a nonzero value during simulation.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateNystrom(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Step,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/**
 * @brief Simulates a network using the adaptive explicit Dormand-Prince method of order 5(4).
 *
 * @param[in] _Network
 * Target network to simulate.
 *
 * @param[in] _Time
 * Duration of the simulation. Must be greater than zero.
 *
 * @param[in] _Atol
 * Absolute tolerance (number of correct decimal places). Must be greater than zero.
 *
 * @param[in] _Rtol
 * Relative tolerance (number of correct digits). Must be greater than zero.
 *
 * @param[in] _Tstops
 * Array containing times the solver must evaluate. May be 0, in which case _Tcount must also be 0.
 *
 * @param[in] _Tcount
 * Length of _Tstops array. Must be greater than zero if _Tstops is provided, and 0 otherwise.
 *
 * @param[in] _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns @ref NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains stochastic agents as it is a purely deterministic method.
 *
 * @warning
 * The function does not verify the capacity of the _Tstops array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : _Time <= 0.
 * - @ref NDXL_VALUE_ERR : _Atol <= 0.
 * - @ref NDXL_VALUE_ERR : _Rtol <= 0.
 * - @ref NDXL_VALUE_ERR : _Tstops is NULL and _Tcount is nonzero, or _Tstops is not NULL and _Tcount is 0.
 * - @ref NDXL_SIM_ERR : _Network contains no agents.
 * - @ref NDXL_SIM_ERR : _Network contains stochastic agents, one of the stochastic methods should be used.
 * - @ref NDXL_SIM_ERR : A dynamics, noise, or callback function returned a nonzero value during simulation.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateDopri(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/**
 * @brief Simulates a network using the adaptive explicit Cash-Karp method of order 5(4).
 *
 * @param[in] _Network
 * Target network to simulate.
 *
 * @param[in] _Time
 * Duration of the simulation. Must be greater than zero.
 *
 * @param[in] _Atol
 * Absolute tolerance (number of correct decimal places). Must be greater than zero.
 *
 * @param[in] _Rtol
 * Relative tolerance (number of correct digits). Must be greater than zero.
 *
 * @param[in] _Tstops
 * Array containing times the solver must evaluate. May be NULL, in which case _Tcount must also be 0.
 *
 * @param[in] _Tcount
 * Length of _Tstops array. Must be greater than zero if _Tstops is provided, and 0 otherwise.
 *
 * @param[in] _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns @ref NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains stochastic agents as it is a purely deterministic method.
 *
 * @warning
 * The function does not verify the capacity of the _Tstops array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : _Time <= 0.
 * - @ref NDXL_VALUE_ERR : _Atol <= 0.
 * - @ref NDXL_VALUE_ERR : _Rtol <= 0.
 * - @ref NDXL_VALUE_ERR : _Tstops is NULL and _Tcount is nonzero, or _Tstops is not NULL and _Tcount is 0.
 * - @ref NDXL_SIM_ERR : _Network contains no agents.
 * - @ref NDXL_SIM_ERR : _Network contains stochastic agents, one of the stochastic methods should be used.
 * - @ref NDXL_SIM_ERR : A dynamics, noise, or callback function returned a nonzero value during simulation.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateRkck(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/**
 * @brief Simulates a network using the static explicit Euler-Maruyama method of strong order 0.5.
 *
 * @param[in] _Network
 * Target network to simulate.
 *
 * @param[in] _Time
 * Duration of the simulation. Must be greater than zero.
 *
 * @param[in] _Step
 * Step size. Must be greater than zero.
 *
 * @param[in] _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns @ref NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains no stochastic agents as it is a stochastic method.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : _Time <= 0.
 * - @ref NDXL_VALUE_ERR : _Step <= 0.
 * - @ref NDXL_SIM_ERR : _Network contains no agents.
 * - @ref NDXL_SIM_ERR : _Network contains no stochastic agents, one of the deterministic methods should be used.
 * - @ref NDXL_SIM_ERR : A dynamics, noise, or callback function returned a nonzero value during simulation.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateRalston
 * @see ndxlNetworkSimulateNystrom
 * @see ndxlNetworkSimulateDopri
 * @see ndxlNetworkSimulateRkck
 * @see ndxlNetworkSimulateRadau
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateEuler(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Step,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/**
 * @brief Simulates a network using the adaptive explicit Euler-Heun method of strong order 1(0.5). Uses rejection sampling with memory (RSwM) to preserve Wiener path distributions.
 *
 * @param[in] _Network
 * Target network to simulate.
 *
 * @param[in] _Time
 * Duration of the simulation. Must be greater than zero.
 *
 * @param[in] _Atol
 * Absolute tolerance (number of correct decimal places). Must be greater than zero.
 *
 * @param[in] _Rtol
 * Relative tolerance (number of correct digits). Must be greater than zero.
 *
 * @param[in] _Tstops
 * Array containing times the solver must evaluate. May be NULL, in which case _Tcount must also be 0.
 *
 * @param[in] _Tcount
 * Length of _Tstops array. Must be greater than zero if _Tstops is provided, and 0 otherwise.
 *
 * @param[in] _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns @ref NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains no stochastic agents as it is a stochastic method.
 *
 * @warning
 * The function does not verify the capacity of the _Tstops array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : _Time <= 0.
 * - @ref NDXL_VALUE_ERR : _Atol <= 0.
 * - @ref NDXL_VALUE_ERR : _Rtol <= 0.
 * - @ref NDXL_VALUE_ERR : _Tstops is NULL and _Tcount is nonzero, or _Tstops is not NULL and _Tcount is 0.
 * - @ref NDXL_SIM_ERR : _Network contains no agents.
 * - @ref NDXL_SIM_ERR : _Network contains no stochastic agents, one of the deterministic methods should be used.
 * - @ref NDXL_SIM_ERR : A dynamics, noise, or callback function returned a nonzero value during simulation.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateRalston
 * @see ndxlNetworkSimulateNystrom
 * @see ndxlNetworkSimulateDopri
 * @see ndxlNetworkSimulateRkck
 * @see ndxlNetworkSimulateRadau
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateHeun(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

//_NDXLIMP_ const int _CDECL ndxlNetworkSimulateNorsett(
//	_In_ _Notnull_ NDXLNetwork* _Network,
//	_In_ const double _Time,
//	_In_ const double _Step,
//	_In_ NDXLNetworkCallback _Callback
//) _NOEXCEPT;

/**
 * @brief Simulates a network using the adaptive implicit Radau IIA method of order 5(3).
 *
 * @param[in] _Network
 * Target network to simulate.
 *
 * @param[in] _Time
 * Duration of the simulation. Must be greater than zero.
 *
 * @param[in] _Atol
 * Absolute tolerance (number of correct decimal places). Must be greater than zero.
 *
 * @param[in] _Rtol
 * Relative tolerance (number of correct digits). Must be greater than zero.
 *
 * @param[in] _Tstops
 * Array containing times the solver must evaluate. May be NULL, in which case _Tcount must also be 0.
 *
 * @param[in] _Tcount
 * Length of _Tstops array. Must be greater than zero if _Tstops is provided, and 0 otherwise.
 *
 * @param[in] _Callback
 * External discrete callback called once at local simulation time of 0, then again after each integration step. May be NULL.
 *
 * @return
 * Returns NDXL_OK if _Network is successfully integrated over _Time, otherwise returns @ref NDXL_ERR.
 *
 * @note
 * This method cannot be used if _Network contains stochastic agents as it is a purely deterministic method.
 *
 * @warning
 * The function does not verify the capacity of the _Tstops array. Providing an inadequately sized array can result in out-of-bounds writes and undefined behaviour.
 *
 * This method relies on network-wide Jacobian evaluations, meaning memory usage scales with the number of agents squared, as opposed to the linear scaling seen in explicit methods.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Network is NULL.
 * - @ref NDXL_VALUE_ERR : _Time <= 0.
 * - @ref NDXL_VALUE_ERR : _Atol <= 0.
 * - @ref NDXL_VALUE_ERR : _Rtol <= 0.
 * - @ref NDXL_VALUE_ERR : _Tstops is NULL and _Tcount is nonzero, or _Tstops is not NULL and _Tcount is 0.
 * - @ref NDXL_SIM_ERR : _Network contains no agents.
 * - @ref NDXL_SIM_ERR : _Network contains stochastic agents, one of the stochastic methods should be used.
 * - @ref NDXL_SIM_ERR : A dynamics, noise, or callback function returned a nonzero value during simulation.
 *
 * @see NDXLNetworkCallback
 * @see ndxlNetworkSimulateEuler
 * @see ndxlNetworkSimulateHeun
*/
_NDXLIMP_ const int _CDECL ndxlNetworkSimulateRadau(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

/** @}*/

#ifdef __cplusplus
}
#endif

#endif // _NDXL_NETWORK_
/**
 * @file ndxl_vertex.h
 *
 * @brief Public API for NDXL vertex creation, state and parameter management, and nearest neighbour traversal.
 *
 * Declares the NDXLVertex object, representing a node in a network. Provides functions for vertex construction, destruction, nearest neighbour traversal, and state and parameter querying.
 *
 * @copyright Copyright (c) 2026, Ethan Codron.
 *
 * Licensed under the GNU General Public License v3.0 only.
 * See the LICENSE file in the repository root.
*/

#ifndef _NDXL_VERTEX_
#define _NDXL_VERTEX_

#include "../ndxlcore.h"

/**
 * @defgroup vertex_dnc Dynamics, Noise, and Callbacks
 * @ingroup ndxl_vertex
 * @brief Dynamics, noise, and callback functions for vertex objects.
 *
 * @{
*/

/**
 * @brief Vertex dynamics function.
 *
 * Defines a user-supplied function that governs how the state of a vertex evolves over time.
 *
 * @param[in] _Vertex
 * The calling vertex.
 *
 * @param[out] _dY
 * Flat output vector containing the time derivative of the calling vertex. ndxlGetVertexOrder(calling_vertex) total elements must be written to it, even if those elements are zero.
 *
 * @param[in] _Y
 * Flat input vector containing the current Runge-Kutta stage of the calling vertex. Is guaranteed to have ndxlGetVertexOrder(calling_vertex) elements.
 *
 * @param[in] _Time
 * Local simulation time of the calling vertex, offset by a fraction of the step size corresponding to the next Runge-Kutta stage. Note that this is generally NOT equivalent to ndxlGetVertexTime(calling_vertex), which only updates on a successful step.
 *
 * @param[in] _Params
 * User-defined parameters associated with the calling vertex.
 *
 * @return
 * Integer indicating function success (zero) or failure (any nonzero value).
 *
 * @note
 * If this function returns a nonzero value during simulation, NDXL terminates the simulation, and internally raises a @ref NDXL_SIM_ERR.
 *
 * @warning
 * The function does not verify the number of elements written to the output array. Not writing to each element can result in garbage values or stale states being propagated. Writing beyond ndxlGetVertexOrder(calling_vertex) elements can result in out-of-bounds writes and undefined behaviour.
 *
 * @see ndxlGetVertexOrder
*/
typedef int(*NDXLVertexDynamics)(NDXLVertex* _Vertex, double* _dY, const double* _Y, double _Time, void* _Params);

/**
 * @brief Vertex noise function.
 *
 * Defines a user-supplied function that provides the amplitude of noise, for each element in its state vector, for each independent Wiener process the vertex is driven by.
 *
 * @param[in] _Vertex
 * The calling vertex.
 *
 * @param[out] _dG
 * Flat output matrix containing the noise amplitudes, where element ij (at index i * ndxlGetVertexChannelCount(calling_vertex) + j) represents the amplitude of the j-th Wiener process on the i-th state-vector element. ndxlGetVertexOrder(calling_vertex) x ndxlGetVertexChannelCount(calling_vertex) total elements must be written to it, even if those elements are zero.
 *
 * @param[in] _Y
 * Flat input vector containing the current Runge-Kutta stage of the calling vertex. Is guaranteed to have ndxlGetVertexOrder(calling_vertex) elements.
 *
 * @param[in] _Time
 * Local simulation time of the calling vertex, offset by a fraction of the step size corresponding to the next Runge-Kutta stage. Note that this is generally NOT equivalent to ndxlGetVertexTime(calling_vertex), which only updates on a successful step.
 *
 * @param[in] _Params
 * User-defined parameters associated with the calling vertex.
 *
 * @return
 * Integer indicating function success (zero) or failure (any nonzero value).
 *
 * @note
 * If this function returns a nonzero value during simulation, NDXL terminates the simulation, and internally raises a @ref NDXL_SIM_ERR.
 *
 * @warning
 * The function does not verify the number of elements written to the output array. Not writing to each element can result in garbage values or stale states being propagated. Writing beyond ndxlGetVertexOrder(calling_vertex) x ndxlGetVertexChannelCount(calling_vertex) elements can result in out-of-bounds writes and undefined behaviour.
 *
 * @see ndxlGetVertexOrder
 * @see ndxlGetVertexChannelCount
*/
typedef int(*NDXLVertexNoise)(NDXLVertex* _Vertex, double* _dG, const double* _Y, double _Time, void* _Params);

/**
 * @brief Vertex callback function.
 *
 * Defines a user-supplied discrete callback that is invoked during simulation of the vertex's host network.
 *
 * @param[in] _Vertex
 * The calling vertex.
 *
 * @param[in,out] _Y
 * Flat vector containing the most recently accepted state vector of the calling vertex. Is guaranteed to have ndxlGetVertexOrder(calling_vertex) elements, and may be modified in-place.
 *
 * @param[in] _Time
 * Current simulation time of the calling vertex, equivalent to ndxlGetVertexTime(calling_vertex).
 *
 * @param[in] _Params
 * User-defined parameters associated with the calling vertex.
 *
 * @return
 * Integer indicating function success (zero) or failure (any nonzero value).
 *
 * @note
 * If this function returns a nonzero value during simulation, NDXL terminates the simulation, and internally raises a @ref NDXL_SIM_ERR.
 *
 * @warning
 * Writing beyond ndxlGetVertexOrder(calling_vertex) elements to the state vector can result in out-of-bounds writes and undefined behaviour.
 *
 * @see ndxlGetVertexOrder
*/
typedef int(*NDXLVertexCallback)(NDXLVertex* _Vertex, double* _Y, double _Time, void* _Params);

/** @}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup vertex_creation Constructor and Destructor
 * @ingroup ndxl_vertex
 * @brief Functions for creating and destroying vertices.
 *
 * @{
*/

/**
 * @brief Creates a vertex in a network and returns a pointer to it.
 *
 * @param[in] _Network
 * Network that will own this vertex. This also dictates which noise channels can affect this vertex.
 * 
 * @param[in] _State
 * Flat initial state vector at time 0. May be NULL, in which case a zero-vector of size max(1, _Dimension) is assigned by default.
 * 
 * @param[in] _Dimension
 * Size of state vector. If _State is provided, must be nonzero.
 * 
 * @param[in] _Dynamics
 * The dynamics function describing how the state vector evolves over time. May be NULL, in which case the time derivative is zero (static vertex).
 * 
 * @param[in] _Noise
 * The noise function describing the noise amplitude over time. May be NULL, in which case _Channels must be NULL and _Nchannels must be 0.
 * 
 * @param[in] _Callback
 * External discrete callback called once after each successful integration step. May be NULL.
 * 
 * @param[in] _Atol
 * Absolute tolerance (number of correct decimal places) the state of this vertex must be calculated to during integration. Must be greater than or equal to zero; if zero, the owning network's absolute tolerance is used.
 * 
 * @param[in] _Rtol
 * Relative tolerance (number of correct digits) the state of this vertex must be calculated to during integration. Must be greater than or equal to zero; if zero, the owning network's relative tolerance is used.
 * 
 * @param[in] _Params
 * Pointer to extra memory used by the vertex. May be NULL.
 *
 * @return
 * If successful, returns a new NDXLVertex pointer, otherwise returns NULL.
 *
 * @note
 * The local simulation time of a newly created vertex is always initialised 0, and only updates on a successful integration step.
 * 
 * @note
 * Unlike network callbacks, vertex callbacks are NOT called at time 0.
 *
 * @warning
 * The function does not verify the capacity of the supplied state array, and expects it to contain at least _Dimension elements. Incorrect array sizes or an inconsistent _Dimension argument can result in out-of-bounds reads and undefined behaviour.
 * 
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the vertex uses it. The user remains responsible for managing any allocations.
 *
 * @see NDXLVertexDynamics
 * @see NDXLVertexNoise
 * @see NDXLVertexCallback
 * @see ndxlDestroyVertex
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlCreateVertex(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_reads_bytes_opt_(_Dimension * sizeof(double)) const double* const _State,
	_In_ const size_t _Dimension,
	_In_ _Maybenull_ NDXLVertexDynamics _Dynamics,
	_In_ _Maybenull_ NDXLVertexNoise _Noise,
	_In_reads_bytes_opt_(_Nchannels * sizeof(NDXLChannel*)) NDXLChannel** _Channels,
	_In_ const size_t _Nchannels,
	_In_ _Maybenull_ NDXLVertexCallback _Callback,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/**
 * @brief Destroys a vertex created using ndxlCreateVertex().
 *
 * @param[in,out] _Vertex
 * Pointer to NDXLVertex pointer.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise destroys the vertex and returns NDXL_OK.
 * 
 * @warning
 * Destroying a vertex also destroys all edges connected to that vertex. All pointers to the vertex and its edges become invalid. The memory associated with the relevant objects' user-defined `void* _Params` pointers is not handled by the destructors, and must be managed explicitly by the user.
 * 
 * @warning
 * Using the vertex after it has been destroyed can result in undefined behaviour. Discard all references!!
 * 
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * 
 * @see ndxlCreateVertex
*/
_NDXLIMP_ const int _CDECL ndxlDestroyVertex(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup vertex_getters Getters
 * @ingroup ndxl_vertex
 * @brief Getter functions for querying vertex properties
 *
 * @{
*/

/**
 * @brief Gets the callback function associated with a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the callback of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the callback function associated with _Vertex.
 * 
 * @note
 * A return value of NULL does not necessarily indicate an error, as the callback function could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * 
 * @see ndxlGetLastErrorCode
 * @see ndxlSetVertexCallback
*/
_NDXLIMP_ NDXLVertexCallback _CDECL ndxlGetVertexCallback(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the number of edges (directed and undirected) connected to a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the degree of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the total number of edges connected to _Vertex.
 * 
 * @note
 * Not to be confused with ndxlGetVertexDegreeIn and ndxlGetVertexDegreeOut, which return the number of edges flowing into and out of _Vertex, respectively.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * 
 * @see ndxlGetVertexDegreeIn
 * @see ndxlGetVertexDegreeOut
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexDegree(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the number of edges flowing into a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the in-degree of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the number of edges flowing into _Vertex. Undirected edges are counted as flowing in.
 *
 * @note
 * Not to be confused with ndxlGetVertexDegree and ndxlGetVertexDegreeOut, which return the total number of edges and number of edges flowing out of _Vertex, respectively.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetVertexDegree
 * @see ndxlGetVertexDegreeOut
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexDegreeIn(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the number of edges flowing out from a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the out-degree of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the number of edges flowing out of _Vertex. Undirected edges are counted as flowing out.
 *
 * @note
 * Not to be confused with ndxlGetVertexDegree and ndxlGetVertexDegreeIn, which return the total number of edges and number of edges flowing into _Vertex, respectively.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetVertexDegree
 * @see ndxlGetVertexDegreeIn
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexDegreeOut(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the size of a vertex's state vector.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the dimension of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the size of the state vector of _Vertex.
 *
 * @note
 * If _State was NULL during construction, the vertex is assigned a zero-initialised state vector whose length is determined by the default-state allocation behaviour described in ndxlCreateVertex().
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlCreateVertex
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexDimension(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the dynamics function associated with a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the dynamics of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the dynamics function associated with _Vertex.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the dynamics function could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetVertexDynamics
*/
_NDXLIMP_ NDXLVertexDynamics _CDECL ndxlGetVertexDynamics(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the initial state vector of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the initial state of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the initial state vector assigned to _Vertex.
 *
 * @note
 * If the initial state vector was NULL, _Vertex is assigned a zero-vector of length ndxlGetVertexOrder(_Vertex), meaning this function will still return a valid pointer containing the zero-vector.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlCreateVertex
 * @see ndxlGetVertexOrder
*/
_NDXLIMP_ const double* _CDECL ndxlGetVertexInitialState(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the neighbouring vertex along a specified edge.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the neighbour of.
 * 
 * @param[in] _Edge
 * The edge along which to query the neighbouring vertex.
 *
 * @return
 * If either _Vertex or _Edge are NULL, or _Vertex is not connected to _Edge, returns NULL, otherwise returns the vertex that is connected to _Vertex by _Edge.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Edge is NULL.
 * - @ref NDXL_VALUE_ERR : _Edge is not connected to _Vertex
 * 
 * @see ndxlCreateEdge
*/
_NDXLIMP_ NDXLVertex* _CDECL ndxlGetVertexNeighbour(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the dimension of a neighbouring vertex along a specified edge.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the neighbour dimension of.
 *
 * @param[in] _Edge
 * The edge along which to query the dimension of the neighbouring vertex.
 *
 * @return
 * If either _Vertex or _Edge are NULL, or _Vertex is not connected to _Edge, returns @ref NDXL_ERR, otherwise returns the dimension of the vertex that is connected to _Vertex by _Edge.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Edge is NULL.
 * - @ref NDXL_VALUE_ERR : _Edge is not connected to _Vertex
 *
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const size_t _CDECL ndxlGetVertexNeighbourDimension(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the time-delayed state vector of a neighbouring vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the neighbour state of.
 *
 * @param[in] _Edge
 * The edge connecting _Vertex to its neighbour, the latter of which will be queried for its time-delayed state.
 * 
 * @return
 * If either _Vertex or _Edge are NULL, or _Edge is not connected to _Vertex, returns NULL, otherwise returns the delayed state vector of the neighbouring vertex to _Vertex along _Edge.
 *
 * @note
 * If the delay of _Edge is zero, this is equivalent to calling ndxlGetVertexState(ndxlGetVertexNeighbour(_Vertex, _Edge)).
 * 
 * @note
 * If the delay of _Edge is nonzero, this function uses historical states stored by _Edge to lookup the state of the neighbouring vertex at time t - ndxlGetEdgeDelay(_Edge). If the requested time does not fall exactly on a stored state, the function performs a cubic Hermite interpolation between the two surrounding states and returns the interpolated state.
 *
 * @note
 * In both cases, the returned pointer is shared, meaning it remains valid over the lifetime of the vertex, but the stored values will change on subsequent calls to this function.
 * 
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref  NDXL_ARG_ERR : _Edge is NULL.
 * - @ref NDXL_VALUE_ERR : _Edge is not connected to _Vertex.
 *
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const double* _CDECL ndxlGetVertexNeighbourState(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

/**
 * @brief Gets the owning network of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the network of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the NDXLNetwork pointer on which _Vertex was created.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
*/
_NDXLIMP_ NDXLNetwork* _CDECL ndxlGetVertexNetwork(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the noise function associated with a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the noise of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the noise function associated with _Vertex.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the noise function could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetVertexNoise
*/
_NDXLIMP_ NDXLVertexNoise _CDECL ndxlGetVertexNoise(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the user-defined `void* _Params` of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the parameters of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns the user-defined `void* _Params` of _Vertex.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as `void* _Params` could be NULL. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetLastErrorCode
 * @see ndxlSetVertexParams
*/
_NDXLIMP_ void* _CDECL ndxlGetVertexParams(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/**
 * @brief Gets the local simulation time of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the time of.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise returns the local simulation time of _Vertex.
 *
 * @note
 * The local simulation time only updates on a successful simulation step.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 * 
 * @see @ref network_simulation "Network Simulation"
*/
_NDXLIMP_ const double _CDECL ndxlGetVertexTime(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup vertex_setters Setters
 * @ingroup ndxl_vertex
 * @brief Setter functions for updating vertex properties.
 *
 * @{
*/

/**
 * @brief Sets the callback function of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to update the callback of.
 * 
 * @param[in] _Callback
 * The new callback that _Vertex must use. May be NULL.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise updates the callback of _Vertex, and returns NDXL_OK.
 * 
 * @warning
 * This function should not be called from within a dynamics or noise function, as these may be invoked multiple times during a simulation step.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
 * @see ndxlGetVertexCallback
*/
_NDXLIMP_ const int _CDECL ndxlSetVertexCallback(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Maybenull_ NDXLVertexCallback _Callback
) _NOEXCEPT;

/**
 * @brief Sets the dynamics function of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to update the dynamics of.
 *
 * @param[in] _Dynamics
 * The new dynamics that _Vertex must use. May be NULL.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise updates the dynamics of _Vertex, and returns NDXL_OK.
 *
 * @warning
 * This function should never be used from within a dynamics or noise function, as these may be invoked multiple times during a simulation step. Modifications to vertex properties should instead be performed from within callback functions, which are invoked after successful integration steps.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
 * @see ndxlGetVertexDynamics
*/
_NDXLIMP_ const int _CDECL ndxlSetVertexDynamics(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Maybenull_ NDXLVertexDynamics _Dynamics
) _NOEXCEPT;

/**
 * @brief Sets the noise function of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to update the noise of.
 *
 * @param[in] _Noise
 * The new noise that _Vertex must use. May be NULL.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise updates the noise of _Vertex, and returns NDXL_OK.
 *
 * @warning
 * This function should never be used from within a dynamics or noise function, as these may be invoked multiple times during a simulation step. Modifications to vertex properties should instead be performed from within callback functions, which are invoked after successful integration steps.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
 * @see ndxlGetVertexNoise
*/
_NDXLIMP_ const int _CDECL ndxlSetVertexNoise(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Maybenull_ NDXLVertexNoise _Noise
) _NOEXCEPT;

/**
 * @brief Sets the user-defined `void* _Params` of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to update the parameters of.
 *
 * @param[in] _Params
 * Pointer to extra memory used by the vertex. May be NULL.
 *
 * @return
 * If _Vertex is NULL, returns @ref NDXL_ERR, otherwise updates the parameters of _Vertex, and returns NDXL_OK.
 *
 * @warning
 * NDXL stores the supplied `void* _Params` pointer without copying the associated data or managing its lifetime. The pointed-to memory must remain valid for as long as the vertex uses it.
 *
 * @warning
 * This function replaces the vertex's existing parameter pointer without freeing or otherwise managing the memory it previously referenced. The user remains responsible for managing both the previous and new allocations.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see ndxlGetVertexParams
*/
_NDXLIMP_ const int _CDECL ndxlSetVertexParams(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

/** @}*/



/**
 * @defgroup vertex_functions Functions
 * @ingroup ndxl_vertex
 * @brief Functions for operating on vertices.
 *
 * @{
*/

/**
 * @brief Provides directed access to an channel in a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the channel in.
 *
 * @param[in] _Idx
 * Relative index (position) of channel in the array. Must be greater than or equal to zero, and less than ndxlGetVertexNoiseDimension(_Vertex).
 *
 * @return
 * If _Vertex is NULL or contains no channels, or _Idx is out of bounds of the underlying array, returns NULL, otherwise returns the NDXLChannel pointer currently at position _Idx in the vertex's underlying channel array.
 *
 * @note
 * The pointer refers to the original object in the vertex, so modifications through it affect the object directly. The returned object can be used to modify or destroy its referenced object without invalidating the object itself.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the vertex could have no channels. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @warning
 * Relative indices are shuffled when agents are destroyed to save memory. Repeated calls with the same index will not necessarily return the same object.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref NDXL_VALUE_ERR : _Idx is out of bounds.
 *
 * @see ndxlGetVertexNoiseDimension
*/
_NDXLIMP_ NDXLChannel* _CDECL ndxlVertexChannel(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ const size_t _Idx
) _NOEXCEPT;

/**
 * @brief Provides directed access to an edge in a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to retrieve the edge in.
 *
 * @param[in] _Idx
 * Relative index (position) of edge in the array. Must be greater than or equal to zero, and less than ndxlGetVertexDegree(_Vertex).
 *
 * @return
 * If _Vertex is NULL or contains no edges, or _Idx is out of bounds of the underlying array, returns NULL, otherwise returns the NDXLEdge pointer currently at position _Idx in the vertex's underlying edge array.
 *
 * @note
 * The pointer refers to the original object in the vertex, so modifications through it affect the object directly. The returned object can be used to modify or destroy its referenced object without invalidating the object itself.
 *
 * @note
 * A return value of NULL does not necessarily indicate an error, as the vertex could have no edges. If unsure, use ndxlGetLastErrorCode() to check.
 *
 * @warning
 * Relative indices are shuffled when agents are destroyed to save memory. Repeated calls with the same index will not necessarily return the same object.
 *
 * @par Errors raised
 * - @ref NDXL_ARG_ERR : _Vertex is NULL.
 * - @ref NDXL_VALUE_ERR : _Idx is out of bounds.
 *
 * @see ndxlGetVertexDegree
*/
_NDXLIMP_ NDXLEdge* _CDECL ndxlVertexEdge(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ const size_t _Idx
) _NOEXCEPT;

/**
 * @brief Checks if an edge exists between two vertices.
 *
 * @param[in] _Source
 * `Source` vertex (vertex 1 in edge creation).
 * 
 * @param[in] _Destination
 * `Destination` vertex (vertex 2 in edge creation).
 *
 * @return
 * If either _Source or _Destination are NULL, returns @ref NDXL_ERR, otherwise returns a bitmask detailing the coupling between _Source and _Destination:
 * - 0, if no coupling exists
 * - 1 (bit 0), if at least one undirected coupling exists.
 * - 3 (bits 0 and 1), if _Source has at least one directed edge to _Destination.
 * - 5 (bits 0 and 2), if _Destination has at least one directed edge to _Source.
 * - 7 (bits 0, 1, and 2) if at least one of each type of coupling exists between _Source and _Destination.
 *
 * @note
 * This function runs in linear time proportional to ndxlGetVertexDegree(_Source). If ndxlGetVertexDegree(_Source) is larger than ndxlGetVertexDegree(_Destination), it may be worth swapping the arguments to reduce the traversal cost. However, doing so reverses the interpretation of the directed-edge flags.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Source is NULL.
 * - @ref  NDXL_ARG_ERR : _Destination is NULL.
 *
 * @see ndxlCreateEdge
*/
_NDXLIMP_ const char _CDECL ndxlVertexHasNeighbour(
	_In_ _Notnull_ NDXLVertex* _Source,
	_In_ _Notnull_ NDXLVertex* _Destination
) _NOEXCEPT;

/**
 * @brief Provides direct access to the internal state vector of a vertex.
 *
 * @param[in] _Vertex
 * Target vertex to see the state of.
 *
 * @return
 * If _Vertex is NULL, returns NULL, otherwise returns a pointer to the internal state vector or, when called from within a dynamics function, the current Runge–Kutta stage vector of _Vertex.
 *
 * @note
 * When called outside a dynamics function, this function returns a pointer to the vertex's primary state vector. When called from within a dynamics function, it instead returns a pointer to the current intermediate Runge–Kutta stage vector. This behaviour allows the same function to be used for directly modifying a vertex's state outside integration and accessing its current integration stage during dynamics evaluation.
 * 
 * @warning
 * Direct modification of the returned vector is supported when it refers to the primary state vector. However, when called from within a dynamics function, the returned pointer refers to the intermediate Runge–Kutta stage vector. Modifying this vector directly is not intended for ordinary state updates and may interfere with the integration process.
 *
 * @par Errors raised
 * - @ref  NDXL_ARG_ERR : _Vertex is NULL.
 *
 * @see @ref network_simulation "Network Simulation"
*/
_NDXLIMP_ double* _CDECL ndxlVertexState(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

/** @}*/

#ifdef __cplusplus
}
#endif

#endif // _NDXL_VERTEX_

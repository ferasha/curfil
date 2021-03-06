# http://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake/4318642#4318642
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/version.cpp" @ONLY)

IF(MDBQ_FOUND)
	SET (MDBQ_FILES hyperopt.cpp)
ELSE()
	SET (MDBQ_FILES )
	SET (MDBQ_LIBRARIES )
ENDIF()

CUDA_ADD_LIBRARY(curfil SHARED random_tree_image_gpu.cu random_tree.cpp image.cpp utils.cpp ndarray_ops.cpp random_tree_image.cpp random_forest_image.cpp import.cpp export.cpp predict.cpp ndarray_ops.cpp train.cpp ${MDBQ_FILES} "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

TARGET_LINK_LIBRARIES(curfil ndarray ${CUDA_LIBRARIES} ${VIGRA_IMPEX_LIBRARY} ${TBB_LIBRARIES} ${Boost_LIBRARIES} ${MDBQ_LIBRARIES})

INSTALL(TARGETS curfil
	DESTINATION "lib"
)

INSTALL(FILES random_tree.h random_tree_image.h random_forest_image.h image.h score.h random_tree_image_gpu.h predict.h import.h export.h utils.h
	DESTINATION "include/curfil"
)

ADD_EXECUTABLE(curfil_train train_main.cpp)
TARGET_LINK_LIBRARIES(curfil_train curfil)

ADD_EXECUTABLE(curfil_predict predict_main.cpp)
TARGET_LINK_LIBRARIES(curfil_predict curfil)

INSTALL(TARGETS curfil_train curfil_predict
    DESTINATION "bin"
)

IF(MDBQ_FOUND)
	ADD_EXECUTABLE(curfil_hyperopt hyperopt_main.cpp)
	TARGET_LINK_LIBRARIES(curfil_hyperopt curfil)
ENDIF()

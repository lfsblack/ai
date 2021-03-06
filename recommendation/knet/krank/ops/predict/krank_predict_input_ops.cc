#include "tensorflow/core/framework/op.h"

namespace tensorflow {

REGISTER_OP("KrankPredictInput")
    .Input("watched_rowkeys: string")
    .Input("rinfo1: float32")
    .Input("rinfo2: float32")
    .Input("target_rowkeys: string")
    .Input("first_video_rowkeys: string")
    .Input("num_targets: int64")
    .Input("is_video: int64")
    .Output("positive_records: int32")
    .Output("negative_records: int32")
    .Output("targets: int32")
    .Output("first_videos: int32")
    .Output("is_target_in_dict: int32")
    .Output("num_positive: int32")
    .Output("num_negative: int32")
    .SetIsStateful()
    .Attr("feature_manager: string = ''")
    .Attr("ws: int = 5")
    .Doc(R"doc(
)doc");

}  // namespace tensorflow


import tensorflow as tf

test_line = [
    '03959e48258956ah 6635a24a17e289ae 2615a3b2c54096aj 0005a21359a912ae',
    '5585a28c29d816aj 5745a2ce833852ae 9605a3384bf404ag 5535a2e0a04600aj 22559af71bd295ab 9605a3384bf404ag 8605979ada5056ae 5775a2d00fc256ao 6505a28edf8329ai 51959e1901a729ae'
]

fasttext_model = tf.load_op_library('../ops/ftlib/fasttext_example_generate_ops.so')
(records, labels) = fasttext_model.fasttext_example_generate(
    train_data_path='../ops/ftlib/train_data.in',
    input=test_line)


sess = tf.Session()
records = sess.run(records)
labels = sess.run(labels)

print("records = \n{}\nlabels = \n{}\n".format(records, labels))
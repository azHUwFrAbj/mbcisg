

def process_and_sort_edges(input_filename, output_filename, vid_column, uid_column, timestamp_column):
    
    with open(input_filename, 'r') as infile:
        lines = infile.readlines() 

    print(f"Processing {len(lines)} lines from {input_filename}")

    raw_edges = []
    for i, line in enumerate(lines):
        if line.startswith("%") or not line.strip():
            continue
        parts = line.strip().split('\t')
        if (len(parts) == 1):
            parts = line.strip().split()
        if len(parts) > max(vid_column, uid_column, timestamp_column):
            u, v = int(parts[vid_column]), int(parts[uid_column])

            if v == u:
                continue
            if timestamp_column < 0 or timestamp_column >= len(parts) or not parts[timestamp_column].isdigit():
                timestamp = -1
            else:
                timestamp = int(parts[timestamp_column])
            raw_edges.append((timestamp, i, u, v))

    unique_ids = sorted(set([u for _, _, u, _ in raw_edges] + [v for _, _, _, v in raw_edges]))
    id_map = {old_id: str(new_id) for new_id, old_id in enumerate(unique_ids)}

    edges = [(t, i, id_map[u], id_map[v]) for t, i, u, v in raw_edges]
    edges.sort()

    with open(output_filename, 'w') as outfile:
        for t, _, u, v in edges:
            outfile.write(f"{u}\t{v}\t{t}\n")


# Extracting edges
process_and_sort_edges('out.dnc-temporalGraph', 'DN.tsv', 0, 1, 3)
# process_and_sort_edges('out.email-EuAll', 'EM.tsv', 0, 1, -1)
# process_and_sort_edges('out.higgs-twitter-social', 'TW.tsv', 0, 1, -1)
# process_and_sort_edges('out.zhishi-baidu-internallink', 'BD.tsv', 0, 1, -1)
# process_and_sort_edges('out.sx-mathoverflow', 'MO.tsv', 0, 1, 3)
# process_and_sort_edges('out.youtube-u-growth', 'YT.tsv', 0, 1, 3)
# process_and_sort_edges('out.link-dynamic-simplewiki', 'SW.tsv', 0, 1, 3)
# process_and_sort_edges('out.elec', 'EL.tsv', 0, 1, 3)
# process_and_sort_edges('out.slashdot-threads', 'SD.tsv', 0, 1, 3)
# process_and_sort_edges('out.sx-stackoverflow', 'SO.tsv', 0, 1, 3)
# process_and_sort_edges('out.epinions', 'EP.tsv', 0, 1, 3)
# process_and_sort_edges('out.lkml-reply', 'LK.tsv', 0, 1, 3)
# process_and_sort_edges('out.enron', 'ER.tsv', 0, 1, 3)
# process_and_sort_edges('out.sx-superuser', 'SU.tsv', 0, 1, 3)
# process_and_sort_edges('out.link-dynamic-dewiki', 'DW.tsv', 0, 1, 3)



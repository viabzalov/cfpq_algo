#include "graph.h"

void GraphRepr_Init(const GraphRepr* graph) {
    ItemMapper_Init((ItemMapper*) &graph->nodes);
    ItemMapper_Init((ItemMapper*) &graph->edges);

    for (int i = 0; i < MAX_GRAMMAR_TERMINALS; ++i) {
        GrB_Matrix_new(&graph->terminal_matrices[i], GrB_BOOL, MAX_GRAPH_SIZE, MAX_GRAPH_SIZE);
    }
}

void GraphRepr_InsertEdge(const GraphRepr* graph, const char* v, const char* edge, const char* to) {
    GrB_Index v_id = ItemMapper_Insert((ItemMapper*) &graph->nodes, v);
    GrB_Index to_id = ItemMapper_Insert((ItemMapper*) &graph->nodes, to);
    GrB_Index edge_id = ItemMapper_Insert((ItemMapper*) &graph->edges, edge);

    assert(v_id != MAX_GRAPH_SIZE && to_id != MAX_GRAPH_SIZE && edge_id != MAX_GRAMMAR_TERMINALS);

    GrB_Matrix_setElement_BOOL(graph->terminal_matrices[edge_id], true, v_id, to_id);

    #ifdef DEBUG
        printf("%s: v = %s %ld\n", __func__, v, v_id);
        printf("%s: edge = %s %ld\n", __func__, edge, edge_id);
        printf("%s: to = %s %ld\n", __func__, to, to_id);
        printf("%s: graph->nodes.count= %ld\n", __func__, graph->nodes.count);
        printf("%s: graph->nodes.count= %ld\n", __func__, graph->edges.count);
    #endif
}

void GraphRepr_DeleteEdge(const GraphRepr* graph, const char* v, const char* edge, const char* to) {
    GrB_Index v_id = ItemMapper_Find((ItemMapper*) &graph->nodes, v);
    GrB_Index edge_id = ItemMapper_Find((ItemMapper*) &graph->edges, edge);
    GrB_Index to_id = ItemMapper_Find((ItemMapper*) &graph->nodes, to);

    assert(v_id != MAX_GRAPH_SIZE && to_id != MAX_GRAPH_SIZE && edge_id != MAX_GRAMMAR_TERMINALS);

    GrB_Matrix_setElement_BOOL(graph->terminal_matrices[edge_id], false, v_id, to_id);
}

void GraphRepr_Load(const GraphRepr* graph, FILE* f) {
    GraphRepr_Init(graph);

    char* line_buf;
    size_t buf_size = 0;

    ItemMapper nodes, edges;
    ItemMapper_Init(&nodes);
    ItemMapper_Init(&edges);

    while (getline(&line_buf, &buf_size, f) != -1) {
        line_buf[strcspn(line_buf, "\n")] = 0;

        char v[MAX_ITEM_NAME_LEN], edge[MAX_ITEM_NAME_LEN], to[MAX_ITEM_NAME_LEN];
        int nitems = sscanf(line_buf, "%s %s %s", v, edge, to);
        assert(nitems == 3);

        GraphRepr_InsertEdge(graph, v, edge, to);
    }
}
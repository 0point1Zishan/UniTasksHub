def dfs(adj, visited, node):
    visited[node] = True
    print(node, end=" ")

    for x in adj[node]:
        if not visited[x]:
            dfs(adj, visited, x)


adj = {
    1 : [6, 7],
    2 : [],
    3 : [4, 1],
    4 : [5, 2],
    5 : [],
    6 : [],
    7 : []
}
n = len(adj)
visited = [False] * (n + 1)
dfs(adj, visited, 3)
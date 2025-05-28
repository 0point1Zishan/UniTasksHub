from collections import deque
def bfs(adj, visited, node):
    q = deque([node])
    visited[node] = True

    while q:
        cur = q.popleft()
        print(cur, end=" ")

        for x in adj[cur]:
            if not visited[x]:
                q.append(x)
                visited[x] = True

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
bfs(adj, visited, 3)
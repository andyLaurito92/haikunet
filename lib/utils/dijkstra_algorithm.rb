class Dijkstra
  
  #I'm expecting my graph as a matrix of 
  #[
  #   ....
  #   [node_i, node_j, weight] 
  #   ....
  #]
  #
  def initialize(start_host, end_host, graph_structure)
    @start = start_host
    @end = end_host

    @path = []

    @infinit = Float::INFINITY

    graph_structure =  graph_structure.unshift [number_of_vertices(graph_structure.dup)]

    read_and_init graph_structure

    get_shortests_path_per_vertex
  end

  # Calculates the number of vertices (unique elements)
  # in a matrix of road
  def number_of_vertices(matrix)
    # Ignoring the weight element (third element)
    matrix = matrix.collect { |x| [x[0], x[1]] }
    # Merging all the path arrays
    matrix = matrix.zip.flatten.compact
    # All the vertices
    @nodes = matrix.uniq.dup
    # Returning the number of unique elements (vertices)
    matrix.uniq.count
  end

  # This method determines the minimum cost of the shortest path
  def cost
    @r[@end]
  end

  # get the shortest path
  def shortest_path
    road(@end)
    @path
  end

  def road(node)
    road(@f[node]) if @f[node] != 0
    @path.push(node)
  end

  def get_shortests_path_per_vertex
    min = @infinit
    pos_min = @infinit

    @nodes.each do |i|
      @r[i] = @road[[@start, i]]
      @f[i] = @start if i != @start && @r[i] < @infinit
    end

    @s[@start] = 1

    @nodes[0..@nodes.size - 2].each do
      min = @infinit

      @nodes.each do |i|
        if @s[i] == 0 && @r[i] < min
          min = @r[i]
          pos_min = i
        end
      end

      @s[pos_min] = 1

      @nodes.each do|j|
        if @s[j] == 0
          if @r[j] > @r[pos_min] + @road[[pos_min, j]]
            @r[j] = @r[pos_min] + @road[[pos_min, j]]
            @f[j] = pos_min
          end
        end
      end
    end
  end

  def read_and_init(a_matrix)
    n = a_matrix.size - 1

    @road = Hash.new(@nodes)
    @r = Hash.new(@nodes)
    @s = Hash.new(@nodes)
    @f = Hash.new(@nodes)

    @nodes.each do |i|
      @r[i] = 0
    end

    @nodes.each do |i|
      @s[i] = 0
    end

    @nodes.each do |i|
      @f[i] = 0
    end

    @nodes.each do |i|
      @nodes.each do |j|
        if i == j
          @road[[i, j]] = 0
        else
          @road[[i, j]] = @infinit
        end
      end
    end

    (1..n).each do |i|
      x = a_matrix[i][0]
      y = a_matrix[i][1]
      c = a_matrix[i][2]
      @road[[x, y]] = c
    end
  end
end
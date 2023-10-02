import networkx as nx
import matplotlib.pyplot as plt

# Parse Verilog code and extract state information
verilog_code = """
module FiveStagePipeline (
  input clk,
  input reset,
  input [7:0] data_in,
  output [7:0] data_out
);

  reg [7:0] stage1;
  reg [7:0] stage2;
  reg [7:0] stage3;
  reg [7:0] stage4;
  reg [7:0] stage5;

  always @(posedge clk or posedge reset) begin
    if (reset) begin
      stage1 <= 8'b0;
      stage2 <= 8'b0;
      stage3 <= 8'b0;
      stage4 <= 8'b0;
      stage5 <= 8'b0;
    end
    else begin
      stage1 <= data_in;
      stage2 <= stage1;
      stage3 <= stage2;
      stage4 <= stage3;
      stage5 <= stage4;
    end
  end

  assign data_out = stage5;

endmodule
"""

# Extract states and transitions from parsed Verilog code
states = []
transitions = []

lines = verilog_code.splitlines()
for line in lines:
    line = line.strip()
    if line.startswith("reg") or line.startswith("wire"):
        state_name = line.split()[2].rstrip(";")
        states.append(state_name)
    elif "<=" in line:
        lhs, rhs = line.split("<=")
        lhs = lhs.strip()
        rhs = rhs.strip(";").strip()
        transitions.append((lhs, rhs))

# Create a directed graph
G = nx.DiGraph()

# Add nodes (states) to the graph
G.add_nodes_from(states)

# Add edges (transitions) to the graph
G.add_edges_from(transitions)

# Visualize the state diagram
pos = nx.spring_layout(G)
nx.draw(G, pos, with_labels=True, node_color='lightblue', node_size=800, font_size=12, edge_color='gray')
plt.title("State Diagram")
plt.show()

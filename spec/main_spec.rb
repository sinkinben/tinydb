describe 'database' do
  before do
    `make build`
  end
  def run_script(commands)
    system("if [ -f mydb.db ]; then rm mydb.db; fi")
    raw_output = nil
    IO.popen("./tinydb mydb.db", "r+") do |pipe|
      commands.each do |command|
        # pipe.puts command
        # ==>
        begin
          pipe.puts command
        rescue Errno::EPIPE
          break
        end
      end

      pipe.close_write

      # Read entire output
      raw_output = pipe.gets(nil)
    end
    raw_output.split("\n")
  end

  it 'inserts and retrieves a row' do
    result = run_script([
      "insert 1 user1 person1@example.com",
      "select",
      ".exit",
    ])
    expect(result).to match_array([
      "tinydb > Executed.",
      "tinydb > (1, user1, person1@example.com)",
      "total 1 rows",
      "Executed.",
      "tinydb > ",
    ])
  end

  it 'allows inserting strings that are the maximum length' do
    long_username = "a"*32
    long_email = "a"*255
    script = [
      "insert 1 #{long_username} #{long_email}",
      "select",
      ".exit",
    ]
    result = run_script(script)
    expect(result).to match_array([
      "tinydb > Executed.",
      "tinydb > (1, #{long_username}, #{long_email})",
      "total 1 rows",
      "Executed.",
      "tinydb > ",
    ])
  end

  it 'prints error message if strings are too long' do
    long_username = "a"*33
    long_email = "a"*256
    script = [
      "insert 1 #{long_username} #{long_email}",
      "select",
      ".exit",
    ]
    result = run_script(script)
    expect(result).to match_array([
      "tinydb > String is too long.",
      "tinydb > total 0 rows",
      "Executed.",
      "tinydb > ",
    ])
  end

  it 'allows printing out the structure of a one-node btree' do
    script = [3, 1, 2].map do |i|
      "insert #{i} user#{i} person#{i}@example.com"
    end
    script << ".btree"
    script << ".exit"
    result = run_script(script)

    expect(result).to match_array([
      "tinydb > Executed.",
      "tinydb > Executed.",
      "tinydb > Executed.",
      "tinydb > Tree:",
      "- leaf (size 3, page 0, parent 0, next 0)",
      "  - 1",
      "  - 2",
      "  - 3",
      "tinydb > "
    ])
  end

  it 'prints constants' do
    script = [
      ".constants",
      ".exit",
    ]
    result = run_script(script)

    expect(result).to match_array([
      "tinydb > Constants:",
      "table row size: 293",
      "  - id:       4",
      "  - username: 33",
      "  - email:    256",
      "common node header size: 6",
      "  - node_type: 1",
      "  - is_root:   1",
      "  - parent:    4",
      "leaf node:",
      "  + header size:      14",
      "    - num_cells: 4",
      "    - next_leaf: 4",
      "  + cell size:        297",
      "    - key size: 4",
      "    - row size: 293",
      "  + spaces for cells: 4082",
      # "  + max cells:        13",
      "internal node:",
      "  + header size:      14",
      "    - num_cells:   4",
      "    - right_child: 4",
      "  + cell size:        8",
      "    - child: 4",
      "    - key:   4",
      "  + spaces for cells: 4082",
      # "  + max cells:        3",
      "tinydb > ",
    ])
  end

  it 'prints an error message if there is a duplicate id' do
    script = [
      "insert 1 user1 person1@example.com",
      "insert 1 user1 person1@example.com",
      "select",
      ".exit",
    ]
    result = run_script(script)
    expect(result).to match_array([
      "tinydb > Executed.",
      "tinydb > Execute Error: Duplicate key.",
      "tinydb > (1, user1, person1@example.com)",
      "total 1 rows",
      "Executed.",
      "tinydb > ",
    ])
  end

  # insert n rows (or with shuffle)
  def run_batch_insertion(n, need_shuffle)
    print("\n#{n} rows, ", "shuffle: #{need_shuffle} \n")
    seq_list = Array.new(n) {|e| e = e + 1}
    input_list = seq_list
    if (need_shuffle)
      input_list = input_list.shuffle
    end
    script = input_list.map do |i| "insert #{i} #{i} #{i}@sjtu.com" end
    script << "select"
    script << ".exit"
    result = run_script(script)
    
    expected_values = seq_list.map do |i| "(#{i}, #{i}, #{i}@sjtu.com)" end
    expected_values[0] = "tinydb > " + expected_values[0]
    expected_values.append(
      "total #{n} rows",
      "Executed.",
      "tinydb > "
    )
    # expected_values.map do |s| print(s + "\n") end
    # result.last(n+3).map do |s| print(s + "\n") end
    expect(result.last(n+3)).to match_array(expected_values)
  end

  # ----------------------------------------
  it '500 rows with increasing id' do
    run_batch_insertion(500, false)
  end

  it '500 rows with shuffling id' do
    run_batch_insertion(500, true)
  end

  # ----------------------------------------
  it '1000 rows with increasing id' do
    run_batch_insertion(1000, false)
  end

  it '1000 rows with shuffling id' do
    run_batch_insertion(1000, true)
  end
  # ----------------------------------------
  it '5000 rows with increasing id' do
    run_batch_insertion(5000, false)
  end

  it '5000 rows with shuffling id' do
    run_batch_insertion(5000, true)
  end

  # ----------------------------------------
  it '6000 rows with increasing id' do
    run_batch_insertion(6000, false)
  end

  it '6000 rows with shuffling id' do
    run_batch_insertion(6000, true)
  end
end
describe 'database' do
  before do
    `rm mydb.db`
  end
  def run_script(commands)
    raw_output = nil
    IO.popen("./tinydb mydb.db", "r+") do |pipe|
      commands.each do |command|
        pipe.puts command
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
      "Executed.",
      "tinydb > ",
    ])
  end

  it 'prints error message when table is full' do
    script = (1..1401).map do |i|
      "insert #{i} user#{i} person#{i}@example.com"
  end
  script << ".exit"
  result = run_script(script)
  expect(result[-2]).to eq('tinydb > Exectue Error: Table full.')
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
    "tinydb > Executed.",
    "tinydb > ",
  ])
end


  it 'keeps data after closing connection' do
  result1 = run_script([
    "insert 1 user1 person1@example.com",
    ".exit",
  ])
  expect(result1).to match_array([
    "tinydb > Executed.",
    "tinydb > ",
  ])
  result2 = run_script([
    "select",
    ".exit",
  ])
  expect(result2).to match_array([
    "tinydb > (1, user1, person1@example.com)",
    "Executed.",
    "tinydb > ",
  ])
end
end
